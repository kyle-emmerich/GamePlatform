#pragma once
#include "Instance/System.h"
#include "Rendering/IRenderer.h"
#include "UISystem.generated.h"

class [[reflect(Engine)]] UISystem : public System, BaseInstance<UISystem> {
	REFLECTION()
public:
	UISystem(Engine* engine) : System(engine) {}

	void Render(Rendering::IRenderer* renderer);
};

REFLECTION_END()
