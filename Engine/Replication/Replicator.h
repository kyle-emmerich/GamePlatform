#pragma once

#include "Instance/Instance.h"
#include "Instance/UUID.h"
#include <unordered_map>
#include "Replicator.generated.h"

class [[reflect(Hidden)]] Replicator : public Instance, BaseInstance<Replicator> {
	REFLECTION()
public:
	Replicator(Engine* engine) : Instance(engine) {}

	[[reflect(ReadOnly)]]
	NetworkPeer* TargetPeer;

protected:
	std::unordered_map<EngineUUID, Instance*> knownInstances;
};

REFLECTION_END()