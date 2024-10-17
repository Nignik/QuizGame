#pragma once

#include <iostream>
#include <thread>
#include <chrono>
#include <cstring>
#include <map>
#include <steam/steamnetworkingsockets.h>
#include <steam/isteamnetworkingutils.h>
#include <filesystem>
#include <fstream>

#include <Serializer.h>
#include <Packets.pb.h>
#include <QuizBlueprint.h>

#include "Quiz.h"
#include "FilesHandler.h"


namespace fs = std::filesystem;

class Server
{
public:
	static Server* s_instance;

	fs::path m_quizDirectory;

	Server(const Server&) = delete;
	Server& operator=(const Server&) = delete;

	static void Init();
	static void Shutdown();

	void LoadQuiz(QuizBlueprint& quizData);

	void Run();
	void SendFilePaths(const HSteamNetConnection& connection);
	void SendQuestion(const HSteamNetConnection& connection);
	void SendVerdict(HSteamNetConnection& connection, bool correct);

private:
	Server() = default;
	~Server() = default;

	std::unique_ptr<Quiz> m_quiz;

	std::map<HSteamNetConnection, std::unique_ptr<QuizCard>> m_players;

	static HSteamListenSocket m_listenSocket;
	static HSteamNetPollGroup m_pollGroup;
	static bool m_isRunning;

	void PollMessages();

	void OnPlayerConnect(HSteamNetConnection& connection);
	void OnPlayerDisconnect(HSteamNetConnection& connection);
	void OnPlayerLogin(const ClientLogin& clientLogin, HSteamNetConnection& connection);
	void OnQuizFilePathsRequest(HSteamNetConnection& connection);
	void OnQuizChangeRequest(const QuizChangeRequest& quizChangeRequest, HSteamNetConnection& connection);
	void OnClientAnswer(const ClientAnswer& clientAnswer, HSteamNetConnection& connection);

	static void OnSteamNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* pInfo);

};