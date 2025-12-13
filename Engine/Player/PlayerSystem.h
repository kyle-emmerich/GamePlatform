#pragma once

#include "Instance/System.h"

class Player;
class PlayerSystem : public System, BaseInstance<PlayerSystem> {
	REFLECTION()
public:
    PlayerSystem(Engine* engine) : System(engine) {}    void HandlePlayerJoin(uint64_t playerId);
    void HandlePlayerLeave(uint64_t playerId);
};

REFLECTION_END()