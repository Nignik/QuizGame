#pragma once

// Client.cpp

#include <iostream>
#include <thread>
#include <chrono>
#include <cstring>
#include <steam/steamnetworkingsockets.h>
#include <steam/isteamnetworkingutils.h>

#include <windows.h>

#include <Serializer.h>
#include <Packets.pb.h>

#include "QuizBlueprint.h"

class Client
{
public:
	static Client* s_instance;
	QuizBlueprint m_quizBlueprint;
	bool m_quizFilePathsAvailable = false;

	Client(const Client&) = delete;
	Client& operator=(const Client&) = delete;

	static void Init(std::string serverAddress);
	static void Shutdown();

	void OnTick();
	bool IsRunning();

	void SendAnswer(std::string& answer);
	void SendQuizFilePathsRequest();
	void SendQuizChangeRequest();

	void PollMessages();

	bool QuizFilePathsAvailable();
	std::vector<std::string> GetQuizFilePaths();

private:
	Client() = default;
	~Client() = default;

	static HSteamNetConnection m_connection;
	static bool m_isConnected;
	static bool m_isRunning;

	std::string m_name = "maks";

	std::vector<std::string> m_quizFilePaths;

	SteamNetworkingIPAddr m_serverAddress{};

	void OnConnected();
	void OnQuizFilePathsReceived(const QuizFilePaths& paths);
	void OnQuestionReceived(const ServerQuestion& question);
	void OnVerdictReceived(const ServerVerdict& verdict);

	static void OnSteamNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* pInfo);
	
};