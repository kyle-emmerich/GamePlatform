#include "Input/KeyCode.h"

int KeyCodeToScancode(KeyCode key) {
	return static_cast<int>(key);
}

KeyCode ScancodeToKeyCode(int scancode) {
	if (scancode < static_cast<int>(KeyCode::None) || scancode > static_cast<int>(KeyCode::LeftArrow)) {
		return KeyCode::None;
	}
	return static_cast<KeyCode>(scancode);
}