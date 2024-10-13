#include "Serializer.h"

void SerializeMessage(const MessageEnvelope& state, std::string& output)
{
	if (!state.SerializeToString(&output))
	{
		std::cerr << "Failed to serialize player state." << std::endl;
	}
}

void DeserializeMessage(const std::string& input, MessageEnvelope& state)
{
	if (!state.ParseFromString(input))
	{
		std::cerr << "Failed to deserialize player state." << std::endl;
	}
}






