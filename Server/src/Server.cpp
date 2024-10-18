#include "Server.h"

Server* Server::s_instance = nullptr;

HSteamListenSocket Server::m_listenSocket = k_HSteamListenSocket_Invalid;
HSteamNetPollGroup Server::m_pollGroup = k_HSteamNetPollGroup_Invalid;
bool Server::m_isRunning = true;

void Server::Init()
{
	s_instance = new Server();

	s_instance->m_scoreSystem = std::make_unique<ScoreSystem>(fs::path("C:/DEV/GamesDev/QuizGame/scores.yaml"));

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

void Server::LoadQuiz(QuizBlueprint& quizData)
{
	fs::path quizPath(quizData.path);
	if (!fs::exists(quizPath))
	{
		std::cout << "Incorrect file path given" << std::endl;
		return;
	}

	std::ifstream file = getCsvFile(quizPath);
	auto shuffle = std::make_shared<MultiplyShuffle>(quizData.repeats);
	m_quiz = std::make_unique<Quiz>(extractCards(file), shuffle);
	for (auto& [conn, playerInfo] : m_players)
	{
		playerInfo->quizCard = std::make_unique<QuizCard>(m_quiz->GetCurrentCard());
		SendQuestion(conn);
	}
	m_quiz->NextCard();
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

void Server::SendPlayerData(const HSteamNetConnection& connection)
{
	MessageEnvelope envelope;
	envelope.set_type(MessageEnvelope::PLAYER_DATA);

	PlayerDataMsg* message = envelope.mutable_playerdata();
	message->set_name(m_players[connection]->name);
	message->set_score(m_scoreSystem->GetPlayerScore(m_players[connection]->name));

	std::string serializedData;
	envelope.SerializeToString(&serializedData);

	SteamNetworkingSockets()->SendMessageToConnection(
		connection, serializedData.c_str(), serializedData.size(), k_nSteamNetworkingSend_Reliable, nullptr);
}

void Server::SendFilePaths(const HSteamNetConnection& connection)
{
	MessageEnvelope envelope;
	envelope.set_type(MessageEnvelope::QUIZ_FILE_PATHS);

	QuizFilePaths* message = envelope.mutable_quizfilepaths();
	for (const auto& entry : fs::directory_iterator(m_quizDirectory))
	{
		message->add_quizpaths(entry.path().string());
	}

	std::string serializedData;
	envelope.SerializeToString(&serializedData);

	SteamNetworkingSockets()->SendMessageToConnection(
		connection, serializedData.c_str(), serializedData.size(), k_nSteamNetworkingSend_Reliable, nullptr);
}

void Server::SendQuestion(const HSteamNetConnection& connection)
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

void Server::SendVerdict(HSteamNetConnection& connection, bool correct)
{
	MessageEnvelope envelope;
	envelope.set_type(MessageEnvelope::SERVER_VERDICT);

	ServerVerdict* message = envelope.mutable_serververdict();
	message->set_answer(m_players[connection]->quizCard->answer);
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
			case MessageEnvelope::CLIENT_LOGIN:				OnPlayerLogin(envelope.clientlogin(), incomingMsg->m_conn);								break;
			case MessageEnvelope::QUIZ_FILE_PATHS_REQUEST:	OnQuizFilePathsRequest(incomingMsg->m_conn);											break;
			case MessageEnvelope::QUIZ_CHANGE_REQUEST:		OnQuizChangeRequest(envelope.quizchangerequest(), incomingMsg->m_conn);					break;
			case MessageEnvelope::CLIENT_ANSWER:			OnClientAnswer(envelope.clientanswer(), incomingMsg->m_conn);							break;

			default:									std::cout << "Reveived invalid packet" << std::endl;										break;
		}

		incomingMsg->Release();
	}
}

void Server::OnPlayerConnect(HSteamNetConnection& connection)
{
	// Accept the connection
	SteamNetworkingSockets()->AcceptConnection(connection);

	// Assign the connection to the poll group
	SteamNetworkingSockets()->SetConnectionPollGroup(connection, m_pollGroup);

	m_players[connection] = nullptr;
}

void Server::OnPlayerDisconnect(HSteamNetConnection& connection)
{
	SteamNetworkingSockets()->CloseConnection(connection, 0, nullptr, false);
}

void Server::OnPlayerLogin(const ClientLogin& clientLogin, HSteamNetConnection& connection)
{
	std::cout << "New connection: " << clientLogin.name() << " from: " << connection << "\n\n";
	m_players[connection] = std::make_shared<PlayerInfo>(connection, clientLogin.name(), nullptr);
}

void Server::OnQuizFilePathsRequest(HSteamNetConnection& connection)
{
	SendFilePaths(connection);
}

void Server::OnQuizChangeRequest(const QuizChangeRequest& quizChangeRequest, HSteamNetConnection& connection)
{
	std::cout << "Received quiz change request" << std::endl;
	
	QuizBlueprint quizData(quizChangeRequest.quizpath(), quizChangeRequest.repeats());
	LoadQuiz(quizData);
}

void Server::OnClientAnswer(const ClientAnswer& clientAnswer, HSteamNetConnection& connection)
{
	if (m_quiz == nullptr)
	{
		std::cout << "No quiz is currently active" << std::endl;
		return;
	}
	std::cout << "Client answer: " << clientAnswer.answer() << " from: " << clientAnswer.name() << '(' << connection << ")\n\n";
	
	bool verdict = clientAnswer.answer() == m_players[connection]->quizCard->answer ? true : false;
	
	m_scoreSystem->IncreasePlayerScore(clientAnswer.name(), verdict);
	SendVerdict(connection, verdict);

	m_players[connection]->quizCard = std::make_unique<QuizCard>(m_quiz->GetCurrentCard());
	SendQuestion(connection);

	m_quiz->NextCard();
}

void Server::OnSteamNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* pInfo)
{
	if (pInfo->m_eOldState == k_ESteamNetworkingConnectionState_None)
	{
		// This is a new connection
		if (pInfo->m_info.m_eState == k_ESteamNetworkingConnectionState_Connecting)
		{
			s_instance->OnPlayerConnect(pInfo->m_hConn);
		}
	}

	if (pInfo->m_info.m_eState == k_ESteamNetworkingConnectionState_ClosedByPeer ||
		pInfo->m_info.m_eState == k_ESteamNetworkingConnectionState_ProblemDetectedLocally)
	{
		s_instance->OnPlayerDisconnect(pInfo->m_hConn);
	}
}

