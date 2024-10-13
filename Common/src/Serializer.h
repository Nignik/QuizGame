#pragma once

#include "Packets.pb.h"

static void SerializeMessage(const MessageEnvelope& state, std::string& output);
static void DeserializeMessage(const std::string& input, MessageEnvelope& state);