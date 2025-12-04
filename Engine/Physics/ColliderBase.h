#pragma once

#include "Instance/ObjectInstance.h"
#include "ColliderBase.generated.h"

class RigidBody;

class [[reflect()]] ColliderBase : public ObjectInstance, BaseInstance<ColliderBase> {
    REFLECTION()
public:
    ColliderBase(Engine* engine) : ObjectInstance(engine) {}

    [[reflect(ReadOnly)]]
    [[summary("The rigid body that this collider is attached to. Determined automatically.")]]
    RigidBody* AttachedBody = nullptr;
};

REFLECTION_END()