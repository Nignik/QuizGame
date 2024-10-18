#include "Client.h"

Client* Client::s_instance = nullptr;

HSteamNetConnection Client::m_connection = k_HSteamNetConnection_Invalid;
bool Client::m_isConnected = false;
bool Client::m_isRunning = true;

void Client::Init(std::string serverAddress, int* score)
{
	s_instance = new Client();

	uint16 port = 27020;
	s_instance->m_score = score;

	SteamDatagramErrMsg errMsg;
	if (!GameNetworkingSockets_Init(nullptr, errMsg))
	{
		std::cerr << "GameNetworkingSockets_Init failed: " << errMsg << "\n";
	}

	// Set the connection status callback function
	SteamNetworkingUtils()->SetGlobalCallback_SteamNetConnectionStatusChanged(OnSteamNetConnectionStatusChanged);

	// Create server address
	s_instance->m_serverAddress.Clear();
	if (!s_instance->m_serverAddress.ParseString(serverAddress.c_str()))
	{
		std::cerr << "Invalid server address.\n";
	}
	s_instance->m_serverAddress.m_port = port;

	// Connect to the server
	m_connection = SteamNetworkingSockets()->ConnectByIPAddress(s_instance->m_serverAddress, 0, nullptr);
	if (m_connection == k_HSteamNetConnection_Invalid)
	{
		std::cerr << "Failed to connect to server.\n";
	}
}

void Client::Shutdown()
{
	if (m_connection != k_HSteamNetConnection_Invalid)
	{
		SteamNetworkingSockets()->CloseConnection(m_connection, 0, nullptr, false);
		m_connection = k_HSteamNetConnection_Invalid;
	}

	GameNetworkingSockets_Kill();

	delete s_instance;
	s_instance = nullptr;
}

void Client::OnTick()
{
	SteamNetworkingSockets()->RunCallbacks();

	PollMessages();
}

bool Client::IsRunning()
{
	return m_isRunning;
}

void Client::SendAnswer(std::string& answer)
{
	MessageEnvelope envelope;
	envelope.set_type(MessageEnvelope::CLIENT_ANSWER);

	ClientAnswer* message = envelope.mutable_clientanswer();
	message->set_name(m_name);
	message->set_answer(answer);

	std::string serializedData;
	envelope.SerializeToString(&serializedData);

	SteamNetworkingSockets()->SendMessageToConnection(
		m_connection, serializedData.c_str(), serializedData.size(), k_nSteamNetworkingSend_Reliable, nullptr);
}

void Client::SendQuizFilePathsRequest()
{
	MessageEnvelope envelope;
	envelope.set_type(MessageEnvelope::QUIZ_FILE_PATHS_REQUEST);

	QuizFilePathsRequest* message = envelope.mutable_quizfilepathsrequest();

	std::string serializedData;
	envelope.SerializeToString(&serializedData);

	SteamNetworkingSockets()->SendMessageToConnection(
		m_connection, serializedData.c_str(), serializedData.size(), k_nSteamNetworkingSend_Reliable, nullptr);
}

void Client::SendQuizChangeRequest()
{
	MessageEnvelope envelope;
	envelope.set_type(MessageEnvelope::QUIZ_CHANGE_REQUEST);

	QuizChangeRequest* message = envelope.mutable_quizchangerequest();
	message->set_quizpath(m_quizBlueprint.path);
	message->set_repeats(m_quizBlueprint.repeats);

	std::string serializedData;
	envelope.SerializeToString(&serializedData);

	SteamNetworkingSockets()->SendMessageToConnection(
		m_connection, serializedData.c_str(), serializedData.size(), k_nSteamNetworkingSend_Reliable, nullptr);
}

void Client::PollMessages()
{
	ISteamNetworkingMessage* incomingMsg = nullptr;
	int numMsgs = SteamNetworkingSockets()->ReceiveMessagesOnConnection(m_connection, &incomingMsg, 1);
	if (numMsgs > 0 && incomingMsg != nullptr)
	{
		std::string receivedData(static_cast<char*>(incomingMsg->m_pData), incomingMsg->m_cbSize);

		MessageEnvelope envelope;
		envelope.ParseFromString(receivedData);
		switch (envelope.type())
		{
			case MessageEnvelope::PLAYER_DATA:			OnPlayerDataReceived(envelope.playerdata());			break;
			case MessageEnvelope::QUIZ_FILE_PATHS:		OnQuizFilePathsReceived(envelope.quizfilepaths());		break;
			case MessageEnvelope::SERVER_QUESTION:		OnQuestionReceived(envelope.serverquestion());			break;
			case MessageEnvelope::SERVER_VERDICT:		OnVerdictReceived(envelope.serververdict());			break;
		}
		
		incomingMsg->Release();
	}
}

bool Client::QuizFilePathsAvailable()
{
	return m_quizFilePathsAvailable;
}

std::vector<std::string> Client::GetQuizFilePaths()
{
	return m_quizFilePaths;
}

void Client::OnSteamNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* pInfo)
{
	if (pInfo->m_info.m_eState == k_ESteamNetworkingConnectionState_Connected)
	{
		s_instance->OnConnected();
		std::cout << "Connected to server.\n";
		m_isConnected = true;
	}
	else if (pInfo->m_info.m_eState == k_ESteamNetworkingConnectionState_ClosedByPeer ||
		pInfo->m_info.m_eState == k_ESteamNetworkingConnectionState_ProblemDetectedLocally)
	{
		std::cout << "Connection closed.\n";
		m_isConnected = false;
		m_isRunning = false;
	}
}

void Client::OnConnected()
{
	std::cout << "Connected to the server" << std::endl;
	MessageEnvelope envelope;
	envelope.set_type(MessageEnvelope::CLIENT_LOGIN);

	ClientLogin* message = envelope.mutable_clientlogin();
	message->set_name(m_name);

	std::string serializedData;
	envelope.SerializeToString(&serializedData);

	SteamNetworkingSockets()->SendMessageToConnection(
		m_connection, serializedData.c_str(), serializedData.size(), k_nSteamNetworkingSend_Reliable, nullptr);
}

void Client::OnPlayerDataReceived(const PlayerDataMsg& playerData)
{
	*m_score = playerData.score();
}

void Client::OnQuizFilePathsReceived(const QuizFilePaths& paths)
{
	for (int i = 0; i < paths.quizpaths_size(); i++)
	{
		m_quizFilePaths.push_back(paths.quizpaths(i));
	}

	m_quizFilePathsAvailable = true;
}

void Client::OnQuestionReceived(const ServerQuestion& question)
{
	std::cout << "Question: " << question.question() << "\n\n";
}

void Client::OnVerdictReceived(const ServerVerdict& verdict)
{
	std::cout << "The correct answer was: " << verdict.answer() << std::endl;
	std::cout << "Your answer is: " << (verdict.correct() ? "correct" : "incorrect") << "\n\n";
	*m_score += verdict.correct();
}
