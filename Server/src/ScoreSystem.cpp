#include "ScoreSystem.h"

ScoreSystem::ScoreSystem(fs::path scoreFile)
	: m_scores(YAML::LoadFile(scoreFile.string()))
{
}

int ScoreSystem::GetPlayerScore(std::string playerName)
{
	int score = m_scores[playerName].as<int>();
	
	return score;
}

void ScoreSystem::SetPlayerScore(std::string playerName, int score)
{
	m_scores[playerName] = score;
}

template <typename T>
void ScoreSystem::IncreasePlayerScore(std::string playerName, T amount)
{
	if (!m_scores[playerName].IsDefined())
		m_scores[playerName] = 0;

	int score = m_scores[playerName].as<int>();
	score += amount;
	m_scores[playerName] = score;
}

template void ScoreSystem::IncreasePlayerScore<int>(std::string playerName, int amount);
template void ScoreSystem::IncreasePlayerScore<bool>(std::string playerName, bool amount);
