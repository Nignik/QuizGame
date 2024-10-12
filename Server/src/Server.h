#pragma once

#include <iostream>
#include <thread>
#include <chrono>
#include <cstring>
#include <steam/steamnetworkingsockets.h>
#include <steam/isteamnetworkingutils.h>

class Server
{
public:
	Server();
	~Server();

	void Run();

private:
	static HSteamListenSocket m_listenSocket;
	static HSteamNetPollGroup m_pollGroup;
	static bool m_isRunning;

	void PollMessages();
	static void OnSteamNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* pInfo);

};