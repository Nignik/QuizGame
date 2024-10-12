#include "Server.h"

HSteamListenSocket Server::m_listenSocket = k_HSteamListenSocket_Invalid;
HSteamNetPollGroup Server::m_pollGroup = k_HSteamNetPollGroup_Invalid;
bool Server::m_isRunning = true;

Server::Server()
{
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

Server::~Server()
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

void Server::PollMessages()
{
	ISteamNetworkingMessage* incomingMsg = nullptr;
	int numMsgs = SteamNetworkingSockets()->ReceiveMessagesOnPollGroup(m_pollGroup, &incomingMsg, 1);
	if (numMsgs > 0 && incomingMsg != nullptr)
	{
		std::string receivedData(static_cast<char*>(incomingMsg->m_pData), incomingMsg->m_cbSize);
		std::cout << "Received message: " << receivedData << " from: " << incomingMsg->m_conn << "\n";

		// Echo the message back to the sender
		HSteamNetConnection conn = incomingMsg->m_conn;
		SteamNetworkingSockets()->SendMessageToConnection(
			conn, incomingMsg->m_pData, incomingMsg->m_cbSize, k_nSteamNetworkingSend_Reliable, nullptr);

		incomingMsg->Release();
	}
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

			std::cout << "Accepted connection from client.\n";
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

