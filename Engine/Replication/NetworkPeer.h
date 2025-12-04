#pragma once

#include "Core/Engine.h"
#include "Instance/Instance.h"
#include "NetworkPeer.generated.h"

class [[reflect(Hidden)]] NetworkPeer : public Instance, BaseInstance<NetworkPeer> {
	REFLECTION()
public:
	NetworkPeer(Engine* engine) : Instance(engine) {}

	[[reflect()]]
	bool IsConnected = false;
	[[reflect()]]
	bool IsSelf = false;

	[[reflect()]]
	bool AcceptsConnections = false;

	[[reflect()]]
	std::string Address;
	[[reflect()]]
	uint16_t Port = 0;
};

REFLECTION_END()