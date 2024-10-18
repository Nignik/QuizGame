#pragma once

#include <filesystem>

#include <yaml-cpp/yaml.h>

namespace fs = std::filesystem;

class ScoreSystem
{
public:
	ScoreSystem(fs::path scoreFile);

	int GetPlayerScore(std::string playerName);
	void SetPlayerScore(std::string playerName, int score);

	template <typename T>
	void IncreasePlayerScore(std::string playerName, T amount);

private:
	YAML::Node m_scores{};
};