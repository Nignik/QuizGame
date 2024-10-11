// Server.cpp

#include <iostream>
#include <thread>
#include <chrono>
#include <cstring>
#include <steam/steamnetworkingsockets.h>
#include <steam/isteamnetworkingutils.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

// Global Variables
HSteamListenSocket listenSocket = k_HSteamListenSocket_Invalid;
HSteamNetPollGroup pollGroup = k_HSteamNetPollGroup_Invalid;
bool isRunning = true;

// Callback Function For accepting a new connection
void OnSteamNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* pInfo)
{
	if (pInfo->m_eOldState == k_ESteamNetworkingConnectionState_None)
	{
		// This is a new connection
		if (pInfo->m_info.m_eState == k_ESteamNetworkingConnectionState_Connecting)
		{
			// Accept the connection
			SteamNetworkingSockets()->AcceptConnection(pInfo->m_hConn);

			// Assign the connection to the poll group
			SteamNetworkingSockets()->SetConnectionPollGroup(pInfo->m_hConn, pollGroup);

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

int main()
{
	SteamDatagramErrMsg errMsg;
	if (!GameNetworkingSockets_Init(nullptr, errMsg))
	{
		std::cerr << "GameNetworkingSockets_Init failed: " << errMsg << "\n";
		return 1;
	}

	// Set the callback function
	SteamNetworkingUtils()->SetGlobalCallback_SteamNetConnectionStatusChanged(OnSteamNetConnectionStatusChanged);

	// Create a listen socket
	SteamNetworkingIPAddr serverAddress;
	serverAddress.Clear();
	serverAddress.m_port = 27020; // Use any desired port

	listenSocket = SteamNetworkingSockets()->CreateListenSocketIP(serverAddress, 0, nullptr);
	if (listenSocket == k_HSteamListenSocket_Invalid)
	{
		std::cerr << "Failed to create listen socket.\n";
		return 1;
	}

	// Create a poll group
	pollGroup = SteamNetworkingSockets()->CreatePollGroup();
	if (pollGroup == k_HSteamNetPollGroup_Invalid)
	{
		std::cerr << "Failed to create poll group.\n";
		return 1;
	}

	std::cout << "Server listening on port " << serverAddress.m_port << "\n";

	// Main Loop
	while (isRunning)
	{
		SteamNetworkingSockets()->RunCallbacks();

		// Poll incoming messages
		ISteamNetworkingMessage* incomingMsg = nullptr;
		int numMsgs = SteamNetworkingSockets()->ReceiveMessagesOnPollGroup(pollGroup, &incomingMsg, 1);
		if (numMsgs > 0 && incomingMsg != nullptr)
		{
			std::string receivedData(static_cast<char*>(incomingMsg->m_pData), incomingMsg->m_cbSize);
			std::cout << "Received message: " << receivedData << " from: " << incomingMsg->GetConnectionUserData() << "\n";

			// Echo the message back to the sender
			HSteamNetConnection conn = incomingMsg->m_conn;
			SteamNetworkingSockets()->SendMessageToConnection(
				conn, incomingMsg->m_pData, incomingMsg->m_cbSize, k_nSteamNetworkingSend_Reliable, nullptr);

			incomingMsg->Release();
		}

		// Sleep for a short duration
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	// Cleanup
	if (listenSocket != k_HSteamListenSocket_Invalid)
	{
		SteamNetworkingSockets()->CloseListenSocket(listenSocket);
		listenSocket = k_HSteamListenSocket_Invalid;
	}

	if (pollGroup != k_HSteamNetPollGroup_Invalid)
	{
		SteamNetworkingSockets()->DestroyPollGroup(pollGroup);
		pollGroup = k_HSteamNetPollGroup_Invalid;
	}

	GameNetworkingSockets_Kill();

	return 0;
}
