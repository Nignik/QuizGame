#pragma once

#include <string_view>
#include <cstdint>

enum class PacketType : uint16_t
{
	None = 0,
	ClientConnect = 1,
	ClientDisconnect = 2,
	ClientAnswer = 3,
	ServerAnswer = 4,
	ServerQuestion = 5,
	ServerShutdown = 6,
};

std::string_view PacketTypeToString(PacketType type);