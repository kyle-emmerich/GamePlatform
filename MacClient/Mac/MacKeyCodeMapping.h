#pragma once

#include "Input/KeyCode.h"
#include <Carbon/Carbon.h>

// Map macOS virtual key codes to Engine KeyCodes
static KeyCode MacKeyCodeToKeyCode(unsigned short keyCode) {
    switch (keyCode) {
        case kVK_ANSI_A: return KeyCode::A;
        case kVK_ANSI_S: return KeyCode::S;
        case kVK_ANSI_D: return KeyCode::D;
        case kVK_ANSI_F: return KeyCode::F;
        case kVK_ANSI_H: return KeyCode::H;
        case kVK_ANSI_G: return KeyCode::G;
        case kVK_ANSI_Z: return KeyCode::Z;
        case kVK_ANSI_X: return KeyCode::X;
        case kVK_ANSI_C: return KeyCode::C;
        case kVK_ANSI_V: return KeyCode::V;
        case kVK_ANSI_B: return KeyCode::B;
        case kVK_ANSI_Q: return KeyCode::Q;
        case kVK_ANSI_W: return KeyCode::W;
        case kVK_ANSI_E: return KeyCode::E;
        case kVK_ANSI_R: return KeyCode::R;
        case kVK_ANSI_Y: return KeyCode::Y;
        case kVK_ANSI_T: return KeyCode::T;
        case kVK_ANSI_1: return KeyCode::Num1;
        case kVK_ANSI_2: return KeyCode::Num2;
        case kVK_ANSI_3: return KeyCode::Num3;
        case kVK_ANSI_4: return KeyCode::Num4;
        case kVK_ANSI_6: return KeyCode::Num6;
        case kVK_ANSI_5: return KeyCode::Num5;
        case kVK_ANSI_Equal: return KeyCode::Equal;
        case kVK_ANSI_9: return KeyCode::Num9;
        case kVK_ANSI_7: return KeyCode::Num7;
        case kVK_ANSI_Minus: return KeyCode::Minus;
        case kVK_ANSI_8: return KeyCode::Num8;
        case kVK_ANSI_0: return KeyCode::Num0;
        case kVK_ANSI_RightBracket: return KeyCode::RightBracket;
        case kVK_ANSI_O: return KeyCode::O;
        case kVK_ANSI_U: return KeyCode::U;
        case kVK_ANSI_LeftBracket: return KeyCode::LeftBracket;
        case kVK_ANSI_I: return KeyCode::I;
        case kVK_ANSI_P: return KeyCode::P;
        case kVK_ANSI_L: return KeyCode::L;
        case kVK_ANSI_J: return KeyCode::J;
        case kVK_ANSI_Quote: return KeyCode::Apostrophe;
        case kVK_ANSI_K: return KeyCode::K;
        case kVK_ANSI_Semicolon: return KeyCode::Semicolon;
        case kVK_ANSI_Backslash: return KeyCode::Backslash;
        case kVK_ANSI_Comma: return KeyCode::Comma;
        case kVK_ANSI_Slash: return KeyCode::Slash;
        case kVK_ANSI_N: return KeyCode::N;
        case kVK_ANSI_M: return KeyCode::M;
        case kVK_ANSI_Period: return KeyCode::Period;
        case kVK_ANSI_Grave: return KeyCode::GraveAccent;
        case kVK_ANSI_KeypadDecimal: return KeyCode::NumPadPeriod;
        case kVK_ANSI_KeypadMultiply: return KeyCode::NumPadMultiply;
        case kVK_ANSI_KeypadPlus: return KeyCode::NumPadAdd;
        case kVK_ANSI_KeypadClear: return KeyCode::NumLockClear; // Closest match
        case kVK_ANSI_KeypadDivide: return KeyCode::NumPadDivide;
        case kVK_ANSI_KeypadEnter: return KeyCode::NumPadEnter;
        case kVK_ANSI_KeypadMinus: return KeyCode::NumPadSubtract;
        case kVK_ANSI_KeypadEquals: return KeyCode::NumPadEqual;
        case kVK_ANSI_Keypad0: return KeyCode::NumPad0;
        case kVK_ANSI_Keypad1: return KeyCode::NumPad1;
        case kVK_ANSI_Keypad2: return KeyCode::NumPad2;
        case kVK_ANSI_Keypad3: return KeyCode::NumPad3;
        case kVK_ANSI_Keypad4: return KeyCode::NumPad4;
        case kVK_ANSI_Keypad5: return KeyCode::NumPad5;
        case kVK_ANSI_Keypad6: return KeyCode::NumPad6;
        case kVK_ANSI_Keypad7: return KeyCode::NumPad7;
        case kVK_ANSI_Keypad8: return KeyCode::NumPad8;
        case kVK_ANSI_Keypad9: return KeyCode::NumPad9;
        case kVK_Return: return KeyCode::Return;
        case kVK_Tab: return KeyCode::Tab;
        case kVK_Space: return KeyCode::Space;
        case kVK_Delete: return KeyCode::Backspace;
        case kVK_Escape: return KeyCode::Escape;
        case kVK_Command: return KeyCode::LeftMeta; // Or RightMeta
        case kVK_Shift: return KeyCode::LeftShift;
        case kVK_CapsLock: return KeyCode::CapsLock;
        case kVK_Option: return KeyCode::LeftAlt;
        case kVK_Control: return KeyCode::LeftControl;
        case kVK_RightShift: return KeyCode::RightShift;
        case kVK_RightOption: return KeyCode::RightAlt;
        case kVK_RightControl: return KeyCode::RightControl;
        case kVK_Function: return KeyCode::None; // No direct mapping usually
        case kVK_F17: return KeyCode::F17;
        case kVK_VolumeUp: return KeyCode::VolumeUp;
        case kVK_VolumeDown: return KeyCode::VolumeDown;
        case kVK_Mute: return KeyCode::Mute;
        case kVK_F18: return KeyCode::F18;
        case kVK_F19: return KeyCode::F19;
        case kVK_F20: return KeyCode::F20;
        case kVK_F5: return KeyCode::F5;
        case kVK_F6: return KeyCode::F6;
        case kVK_F7: return KeyCode::F7;
        case kVK_F3: return KeyCode::F3;
        case kVK_F8: return KeyCode::F8;
        case kVK_F9: return KeyCode::F9;
        case kVK_F11: return KeyCode::F11;
        case kVK_F13: return KeyCode::F13;
        case kVK_F16: return KeyCode::F16;
        case kVK_F14: return KeyCode::F14;
        case kVK_F10: return KeyCode::F10;
        case kVK_F12: return KeyCode::F12;
        case kVK_F15: return KeyCode::F15;
        case kVK_Help: return KeyCode::Help;
        case kVK_Home: return KeyCode::Home;
        case kVK_PageUp: return KeyCode::PageUp;
        case kVK_ForwardDelete: return KeyCode::DeleteForward;
        case kVK_F4: return KeyCode::F4;
        case kVK_End: return KeyCode::End;
        case kVK_F2: return KeyCode::F2;
        case kVK_PageDown: return KeyCode::PageDown;
        case kVK_F1: return KeyCode::F1;
        case kVK_LeftArrow: return KeyCode::LeftArrow;
        case kVK_RightArrow: return KeyCode::RightArrow;
        case kVK_DownArrow: return KeyCode::DownArrow;
        case kVK_UpArrow: return KeyCode::UpArrow;
        default: return KeyCode::None;
    }
}
