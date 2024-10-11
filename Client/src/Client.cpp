#include "Client.h"

HSteamNetConnection Client::m_connection = k_HSteamNetConnection_Invalid;
bool Client::m_isConnected = false;
bool Client::m_isRunning = true;

Client::Client(std::string serverAddress)
{
	uint16 port = 27020; // Should match the server port

	SteamDatagramErrMsg errMsg;
	if (!GameNetworkingSockets_Init(nullptr, errMsg))
	{
		std::cerr << "GameNetworkingSockets_Init failed: " << errMsg << "\n";
	}

	// Set the connection status callback function
	SteamNetworkingUtils()->SetGlobalCallback_SteamNetConnectionStatusChanged(OnSteamNetConnectionStatusChanged);

	// Create server address
	m_serverAddress.Clear();
	if (!m_serverAddress.ParseString(serverAddress.c_str()))
	{
		std::cerr << "Invalid server address.\n";
	}
	m_serverAddress.m_port = port;

	// Connect to the server
	m_connection = SteamNetworkingSockets()->ConnectByIPAddress(m_serverAddress, 0, nullptr);
	if (m_connection == k_HSteamNetConnection_Invalid)
	{
		std::cerr << "Failed to connect to server.\n";
	}
}

Client::~Client()
{
	if (m_connection != k_HSteamNetConnection_Invalid)
	{
		SteamNetworkingSockets()->CloseConnection(m_connection, 0, nullptr, false);
		m_connection = k_HSteamNetConnection_Invalid;
	}

	GameNetworkingSockets_Kill();
}

void Client::OnTick()
{
	SteamNetworkingSockets()->RunCallbacks();

	PollMessages();
	SendMessages();
}

bool Client::IsRunning()
{
	return m_isRunning;
}

void Client::SendMsg(std::string msg)
{
	if (m_isConnected)
	{
		SteamNetworkingSockets()->SendMessageToConnection(
			m_connection, msg.c_str(), msg.size(), k_nSteamNetworkingSend_Reliable, nullptr);
		std::cout << "Sent message: " << msg << "\n";
	}
}

void Client::PollMessages()
{
	ISteamNetworkingMessage* incomingMsg = nullptr;
	int numMsgs = SteamNetworkingSockets()->ReceiveMessagesOnConnection(m_connection, &incomingMsg, 1);
	if (numMsgs > 0 && incomingMsg != nullptr)
	{
		std::string receivedData(static_cast<char*>(incomingMsg->m_pData), incomingMsg->m_cbSize);
		std::cout << "Received message from server: " << receivedData << "\n";
		incomingMsg->Release();
	}
}

void Client::SendMessages()
{
	static auto lastSendTime = std::chrono::steady_clock::now();
	if (m_isConnected && std::chrono::steady_clock::now() - lastSendTime > std::chrono::seconds(5))
	{
		const char* data = "Hello from Client!";
		SteamNetworkingSockets()->SendMessageToConnection(
			m_connection, data, strlen(data), k_nSteamNetworkingSend_Reliable, nullptr);
		lastSendTime = std::chrono::steady_clock::now();
		std::cout << "Sent message: " << data << "\n";
	}
}

void Client::OnSteamNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* pInfo)
{
	if (pInfo->m_info.m_eState == k_ESteamNetworkingConnectionState_Connected)
	{
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