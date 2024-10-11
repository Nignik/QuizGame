#pragma once

// Client.cpp

#include <iostream>
#include <thread>
#include <chrono>
#include <cstring>
#include <steam/steamnetworkingsockets.h>
#include <steam/isteamnetworkingutils.h>

#include <windows.h>

class Client
{
public:
	Client(std::string serverAddress);
	~Client();

	void OnTick();
	bool IsRunning();

	void SendMsg(std::string msg);

	void PollMessages();
	void SendMessages();

private:
	static HSteamNetConnection m_connection;
	static bool m_isConnected;
	static bool m_isRunning;

	SteamNetworkingIPAddr m_serverAddress{};

	static void OnSteamNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* pInfo);
};