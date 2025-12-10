#pragma once
#include "Physics/ColliderBase.h"
#include "BoxCollider.generated.h"

class [[reflect()]] BoxCollider : public ColliderBase, BaseInstance<BoxCollider> {
	REFLECTION()
public:
	BoxCollider(Engine* engine) : ColliderBase(engine) {}

    [[reflect()]]
    [[summary("The size of the box collider.")]]
    Math::Vector3<double> Size;


};

REFLECTION_END()