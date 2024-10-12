#include "PacketTypes.h"

std::string_view PacketTypeToString(PacketType type)
{
	switch (type)
	{
		case PacketType::None:					return "PacketType::None";
		case PacketType::ClientConnect:			return "PacketType::ClientConnect";
		case PacketType::ClientDisconnect:		return "PacketType::ClientDisconnect";
		case PacketType::ClientAnswer:			return "PacketType::ClientAnswer";
		case PacketType::ServerAnswer:			return "PacketType::ServerAnswer";
		case PacketType::ServerQuestion:		return "PacketType::ServerQuestion";
		case PacketType::ServerShutdown:		return "PacketType::ServerShutdown";

		default:								return "PacketType::<Invalid>";
	}

	return "PacketType::<Invalid>";
}