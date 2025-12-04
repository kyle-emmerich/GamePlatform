#pragma once

#include "Instance/Instance.h"
#include "Replication/NetworkPeer.h"
#include "NetworkClient.generated.h"

class [[reflect()]] NetworkClient : public Instance, BaseInstance<NetworkClient> {
	REFLECTION()
public:
	NetworkClient(Engine* engine) : Instance(engine) {}
};

REFLECTION_END()