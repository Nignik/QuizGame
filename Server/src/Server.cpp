#include "Server.h"

Server* Server::s_instance = nullptr;

HSteamListenSocket Server::m_listenSocket = k_HSteamListenSocket_Invalid;
HSteamNetPollGroup Server::m_pollGroup = k_HSteamNetPollGroup_Invalid;
bool Server::m_isRunning = true;

void Server::Init()
{
	s_instance = new Server();

	SteamDatagramErrMsg errMsg;
	if (!GameNetworkingSockets_Init(nullptr, errMsg))
	{
		std::cerr << "GameNetworkingSockets_Init failed: " << errMsg << "\n";
	}

	// Set the callback function
	SteamNetworkingUtils()->SetGlobalCallback_SteamNetConnectionStatusChanged(OnSteamNetConnectionStatusChanged);

	// Create a listen socket
	SteamNetworkingIPAddr serverAddress;
	serverAddress.Clear();
	serverAddress.m_port = 27020; // Use any desired port

	m_listenSocket = SteamNetworkingSockets()->CreateListenSocketIP(serverAddress, 0, nullptr);
	if (m_listenSocket == k_HSteamListenSocket_Invalid)
	{
		std::cerr << "Failed to create listen socket.\n";
	}

	// Create a poll group
	m_pollGroup = SteamNetworkingSockets()->CreatePollGroup();
	if (m_pollGroup == k_HSteamNetPollGroup_Invalid)
	{
		std::cerr << "Failed to create poll group.\n";
	}

	std::cout << "Server listening on port " << serverAddress.m_port << "\n";

	std::vector<QuizCard> cards = {
		{"a", "b"},
		{"ab", "cd"}
	};
	auto shuffle = std::make_shared<RandomShuffle>();

	s_instance->m_quiz = std::make_unique<Quiz>(cards, shuffle);
}

void Server::Shutdown()
{
	if (m_listenSocket != k_HSteamListenSocket_Invalid)
	{
		SteamNetworkingSockets()->CloseListenSocket(m_listenSocket);
		m_listenSocket = k_HSteamListenSocket_Invalid;
	}

	if (m_pollGroup != k_HSteamNetPollGroup_Invalid)
	{
		SteamNetworkingSockets()->DestroyPollGroup(m_pollGroup);
		m_pollGroup = k_HSteamNetPollGroup_Invalid;
	}

	GameNetworkingSockets_Kill();

	delete s_instance;
	s_instance = nullptr;
}

void Server::Run()
{
	while (m_isRunning)
	{
		SteamNetworkingSockets()->RunCallbacks();

		PollMessages();

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

void Server::MessageQuestion(HSteamNetConnection& connection)
{
	MessageEnvelope envelope;
	envelope.set_type(MessageEnvelope::SERVER_QUESTION);

	ServerQuestion* message = envelope.mutable_serverquestion();
	message->set_question(m_quiz->GetCurrentQuestion());

	std::string serializedData;
	envelope.SerializeToString(&serializedData);

	SteamNetworkingSockets()->SendMessageToConnection(
		connection, serializedData.c_str(), serializedData.size(), k_nSteamNetworkingSend_Reliable, nullptr);
}

void Server::MessageVerdict(HSteamNetConnection& connection, bool correct)
{
	MessageEnvelope envelope;
	envelope.set_type(MessageEnvelope::SERVER_VERDICT);

	ServerVerdict* message = envelope.mutable_serververdict();
	message->set_answer(m_players[connection].answer);
	message->set_correct(correct);

	std::string serializedData;
	envelope.SerializeToString(&serializedData);

	SteamNetworkingSockets()->SendMessageToConnection(
		connection, serializedData.c_str(), serializedData.size(), k_nSteamNetworkingSend_Reliable, nullptr);
}

void Server::PollMessages()
{
	ISteamNetworkingMessage* incomingMsg = nullptr;
	int numMsgs = SteamNetworkingSockets()->ReceiveMessagesOnPollGroup(m_pollGroup, &incomingMsg, 1);
	if (numMsgs > 0 && incomingMsg != nullptr)
	{
		std::string receivedData(static_cast<char*>(incomingMsg->m_pData), incomingMsg->m_cbSize);

		MessageEnvelope envelope;
		envelope.ParseFromString(receivedData);
		switch (envelope.type())
		{
			case MessageEnvelope::CLIENT_CONNECT:		OnClientConnect(envelope.clientconnect(), incomingMsg->m_conn);			break;
			case MessageEnvelope::CLIENT_ANSWER:		OnClientAnswer(envelope.clientanswer(), incomingMsg->m_conn);			break;

			default:									std::cout << "Reveived invalid packet" << std::endl;					break;
		}

		incomingMsg->Release();
	}
}

void Server::OnClientConnect(const ClientConnect& clientConnect, HSteamNetConnection& connection)
{
	std::cout << "New connection: " << clientConnect.name() << " from: " << connection << "\n\n";

	m_players[connection] = m_quiz->GetCurrentCard();
	MessageQuestion(connection);

	m_quiz->NextCard();
}

void Server::OnClientAnswer(const ClientAnswer& clientAnswer, HSteamNetConnection& connection)
{
	std::cout << "Client answer: " << clientAnswer.answer() << " from: " << clientAnswer.name() << '(' << connection << ")\n\n";
	
	MessageVerdict(connection, clientAnswer.answer() == m_players[connection].answer ? true : false);

	m_players[connection] = m_quiz->GetCurrentCard();
	MessageQuestion(connection);

	m_quiz->NextCard();
}

void Server::OnSteamNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* pInfo)
{
	if (pInfo->m_eOldState == k_ESteamNetworkingConnectionState_None)
	{
		// This is a new connection
		if (pInfo->m_info.m_eState == k_ESteamNetworkingConnectionState_Connecting)
		{
			// Accept the connection
			SteamNetworkingSockets()->AcceptConnection(pInfo->m_hConn);

			// Assign the connection to the poll group
			SteamNetworkingSockets()->SetConnectionPollGroup(pInfo->m_hConn, m_pollGroup);
		}
	}

	if (pInfo->m_info.m_eState == k_ESteamNetworkingConnectionState_ClosedByPeer ||
		pInfo->m_info.m_eState == k_ESteamNetworkingConnectionState_ProblemDetectedLocally)
	{
		// Clean up the connection
		SteamNetworkingSockets()->CloseConnection(pInfo->m_hConn, 0, nullptr, false);
		std::cout << "Connection closed.\n";
	}
}

