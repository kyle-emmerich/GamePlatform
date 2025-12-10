#include "UI/UISystem.h"
#include "UI/UILayer.h"
#include "Core/Engine.h"
#include "Platform/Viewport.h"

void UISystem::Render(Rendering::IRenderer* renderer) {
	for (Instance* child : engine->Children) {
		if (child->IsA<UILayer>()) {
			//static_cast<UILayer*>(child)->Render(renderer);
		}
	}
}
