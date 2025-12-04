#pragma once

#include <vector>

#include "Instance/System.h"
#include "PhysicsSystem.generated.h"


class [[reflect()]] PhysicsSystem : public System, BaseInstance<PhysicsSystem> {
    REFLECTION()
public:
    PhysicsSystem(Engine* engine) : System(engine) {}
    
    void Update(double deltaTime) override;

protected:
};

REFLECTION_END()