

#include <steam/steamnetworkingsockets.h>
#include <steam/isteamnetworkingutils.h>

#include "QuizCards.h"

struct PlayerInfo
{
	HSteamNetConnection connection{};
	std::string name{};
	std::unique_ptr<QuizCard> quizCard{};
};