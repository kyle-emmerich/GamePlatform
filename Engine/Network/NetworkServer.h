#pragma once

#include "Instance/Instance.h"
#include "Replication/NetworkPeer.h"
#include "NetworkServer.generated.h"

class [[reflect()]] NetworkServer : public Instance, BaseInstance<NetworkServer> {
	REFLECTION()
public:
	NetworkServer(Engine* engine) : Instance(engine) {}
};

REFLECTION_END()