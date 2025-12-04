#pragma once
#include "Instance/Instance.h"
#include "Physics/PhysicalProperties.h"
#include "Math/Transform.h"
#include "Math/Vector3.h"
#include "RigidBody.generated.h"

class [[reflect()]] RigidBody : public Instance, public BaseInstance<RigidBody> {
    REFLECTION()
public:
    RigidBody(Engine* engine);
    virtual ~RigidBody();

    [[reflect()]]
    [[summary("The body's physical properties.")]]   
    PhysicalProperties Properties;

    [[reflect(ReadOnly)]]
    Transform<double> Inertia;

    [[reflect()]]
    Vector3<double> Velocity;

    [[reflect()]]
    Vector3<double> AngularVelocity;
};

REFLECTION_END()