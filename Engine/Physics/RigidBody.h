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
    Math::Transform<double> Inertia;

    [[reflect()]]
    Math::Vector3<double> Velocity;

    [[reflect()]]
    Math::Vector3<double> AngularVelocity;
};

REFLECTION_END()