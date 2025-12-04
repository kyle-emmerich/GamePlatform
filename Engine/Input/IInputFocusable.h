#pragma once

#include "IInputFocusable.generated.h"

class InputSystem;
class Input;

class [[reflect(Interface)]] IInputFocusable {
	REFLECTION()
protected:
	friend class InputSystem;
	friend class Input;

	/// Returns true if focus state changed.
	virtual bool __SetFocused(bool isFocused, Input* instigatingInput) = 0;
	/// Returns true if the focusable can be focused by the given input.
	virtual bool __CanBeFocused(Input* instigatingInput) const = 0;
	/// Returns true if this is currently focused.
	virtual bool __IsFocused() const = 0;
public:
	virtual ~IInputFocusable() = default;

	virtual void CaptureFocus() = 0;
	virtual void ReleaseFocus() = 0;
};

REFLECTION_END()