#pragma once

#include "Instance/Instance.h"
#include "Input/IInputFocusable.h"
#include "Rendering/IRenderer.h"
#include "Math/Rect.h"
#include "Math/UDim2.h"
#include "Math/Transform.h"
#include "UIBase.generated.h"

class [[reflect(Hidden)]] UIBase : public Instance, BaseInstance<UIBase>, protected IInputFocusable {
	REFLECTION()
public:
	UIBase(Engine* engine) : Instance(engine) {}

	virtual void OnRender(const Math::Transform<double>& layerTransform, Rendering::IRenderer* renderer);

	[[reflect()]]
	[[summary("Determines if the UI element and its descendants are visible. Non-visible UI elements do not receive input events.")]]
	bool Visible = true;

	[[reflect()]]
	[[summary("Determines if the UI element and its descendants can receive input events. If false, the UI element will not receive input events, but its children may still receive input events if they have Active set to true.")]]
	bool Active = false;

	[[reflect()]]
	[[summary("Determines if the UI element can be selected. If false, the UI element will not be selectable, but its children may still be selectable if they have Selectable set to true.")]]
	bool Selectable = false;

	[[reflect()]]
	[[summary("Determines if the UI element is currently selected. Only one UI element can be selected at a time. Setting this to true will automatically set all other selectable UI elements' Selected property to false.")]]
	bool Selected = false;

	[[reflect()]]
	[[summary("Determines if the UI element can be focused. If false, the UI element will not be focusable, but its children may still be focusable if they have Focusable set to true.")]]
	bool Focusable = false;

	[[reflect()]]
	[[summary("Determines if the UI element is currently focused. Only one UI element can be focused at a time. Setting this to true will automatically set all other focusable UI elements' Focused property to false.")]]
	bool Focused = false;

	[[reflect()]]
	void CaptureFocus() override;
	[[reflect()]]
	void ReleaseFocus() override;

	[[reflect()]]
	MulticastEvent<Input*> FocusGained;
	[[reflect()]]
	MulticastEvent<Input*> FocusLost;

	[[reflect(Setter)]]
	void SetFocused(bool isFocused);
	[[reflect(Setter)]]
	void SetFocusable(bool isFocusable);

	[[reflect(Derived)]]
	Math::Rect<float> GetAbsoluteBounds();

	[[reflect()]]
	Math::UDim2<float> Position;

	[[reflect()]]
	Math::UDim2<float> Size;

	[[reflect()]]
	float Rotation;

	[[reflect()]]
	int ZIndex;

	[[reflect()]]
	bool ClipsDescendants = false;

protected:
	bool __SetFocused(bool isFocused, Input* instigatingInput) override;
	bool __CanBeFocused(Input* instigatingInput) const override;
	bool __IsFocused() const override;
};

REFLECTION_END()