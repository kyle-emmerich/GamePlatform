#pragma once

#include "Instance/ObjectInstance.h"

class Player : public ObjectInstance, BaseInstance<Player> {
    REFLECTION()
public:
    Player(Engine* engine) : ObjectInstance(engine) {}
    
protected:

};

REFLECTION_END()