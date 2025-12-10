#pragma once

#include "Instance/Instance.h"
#include "Math/Transform.h"
#include "ObjectInstance.generated.h"

class World;

/// @brief Represents a 3D object in the game world.
class [[reflect()]] ObjectInstance : public Instance, BaseInstance<ObjectInstance> {
	REFLECTION()
public:
	ObjectInstance(Engine* engine) : Instance(engine) {}

	[[reflect()]]
	[[summary("The object's world transform.")]]
	Math::Transform<double> WorldTransform;

	[[reflect()]]
	World* GetWorld() const { return world; }

protected:
	World* world;

	void __onParentChanged(Instance* newParent);
};

REFLECTION_END()