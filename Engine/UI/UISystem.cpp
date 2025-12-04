#include "UI/UISystem.h"
#include "UI/UIBase.h"
#include "Core/Engine.h"

void UISystem::Render(Rendering::IRenderer* renderer) {
	for (Instance* child : engine->Children) {
		if (child->IsA<UIBase>()) {
			static_cast<UIBase*>(child)->OnRender(renderer);
		}
	}
}
