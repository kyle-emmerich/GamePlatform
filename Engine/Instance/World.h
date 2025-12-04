#pragma once

#include "Instance/Instance.h"
#include "World.generated.h"

class [[reflect()]] World : public Instance, BaseInstance<World> {
	REFLECTION()
public:
	World(Engine* engine) : Instance(engine) {}

	
};
 
REFLECTION_END()
