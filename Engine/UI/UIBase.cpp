#include "UI/UIBase.h"
#include "Input/InputSystem.h"
#include "Core/Engine.h"

void UIBase::CaptureFocus() {
	if (!Focusable) return;
	InputSystem* inputSystem = GetEngine()->GetSystem<InputSystem>();
	if (inputSystem) {
		inputSystem->Focus(this);
	}
}

void UIBase::ReleaseFocus() {
	InputSystem* inputSystem = GetEngine()->GetSystem<InputSystem>();
	if (inputSystem && inputSystem->FocusedInstance == this) {
		inputSystem->ClearFocus();
	}
}

bool UIBase::__SetFocused(bool isFocused, Input* instigatingInput) {
	SetFocused(isFocused);
	if (Focused) {
		FocusGained.Fire(instigatingInput);
	}
	else {
		FocusLost.Fire(instigatingInput);
	}
	return true;
}

bool UIBase::__CanBeFocused(Input* instigatingInput) const {
	return Focusable;
}

bool UIBase::__IsFocused() const {
	return Focused;
}

void UIBase::SetFocused(bool isFocused) {
	if (isFocused != Focused) {
		Focused = isFocused;
		raisePropChanged(prop_Focused);
	}
}

void UIBase::SetFocusable(bool isFocusable) {
	if (isFocusable != Focusable) {
		Focusable = isFocusable;
		raisePropChanged(prop_Focusable);
	}
}

void UIBase::OnRender(Rendering::IRenderer* renderer) {
	if (!Visible) return;

	for (Instance* child : Children) {
		if (child->IsA<UIBase>()) {
			static_cast<UIBase*>(child)->OnRender(renderer);
		}
	}
}

Math::Rect<float> UIBase::GetAbsoluteBounds() {
	// Placeholder implementation
	Math::Vector2<float> parentPos(0, 0);
	Math::Vector2<float> parentSize(0, 0); // Default to 0 or maybe screen size if we had access to it

	// Try to get parent bounds if it's a UIBase
	if (Parent && Parent->IsA<UIBase>()) {
		Math::Rect<float> parentBounds = ((UIBase*)Parent)->GetAbsoluteBounds();
		parentPos = parentBounds.min;
		parentSize = parentBounds.Size();
	}
	// If parent is not UIBase (e.g. UISystem or ScreenGui equivalent), we might need to get screen size.
	// For now, let's assume 0,0 start and maybe some default size or 0 size.
	// Ideally we'd access the Renderer or Viewport size here.

	float x = parentPos.X + Position.X.Scale * parentSize.X + Position.X.Offset;
	float y = parentPos.Y + Position.Y.Scale * parentSize.Y + Position.Y.Offset;
	float w = Size.X.Scale * parentSize.X + Size.X.Offset;
	float h = Size.Y.Scale * parentSize.Y + Size.Y.Offset;

	return Math::Rect<float>(x, y, x + w, y + h);
}