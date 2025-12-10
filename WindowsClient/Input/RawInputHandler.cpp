#include "Input/RawInputHandler.h"
#include "Core/Engine.h"
#include "Windows/KeyCodeMapping.h"
#include "Input/InputSystem.h"
#include <iostream>

RawInputHandler::RawInputHandler(Engine* engine)
    : engine(engine) {

}
RawInputHandler::~RawInputHandler() {

}

void RawInputHandler::Initialize(PlatformWindow* _window) {
    window = _window;
    HWND hwnd = (HWND)window->GetNativeWindowHandle();

    //Register for keyboard and mouse for now.
    RAWINPUTDEVICE rid[2];
    rid[0].usUsagePage = 0x01; // Generic Desktop Controls
    rid[0].usUsage = 0x06;     // Keyboard
    rid[0].dwFlags = 0;        // RIDEV_NOLEGACY; // Removed to allow WM_CHAR and standard window behavior
    rid[0].hwndTarget = hwnd;

    rid[1].usUsagePage = 0x01; // Generic Desktop Controls
    rid[1].usUsage = 0x02;     // Mouse
    rid[1].dwFlags = 0;        // RIDEV_NOLEGACY; // Removed to allow standard window behavior (moving, resizing, closing)
    rid[1].hwndTarget = hwnd;

    if (!RegisterRawInputDevices(rid, 2, sizeof(RAWINPUTDEVICE))) {
        //registration failed
        std::cerr << "Failed to register raw input devices. Client will not receive keyboard/mouse input." << std::endl;
    }
    

}
void RawInputHandler::Shutdown() {

}

void RawInputHandler::HandleInput(LPARAM lParam) {
    UINT dwSize = 0;
    GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
    if (dwSize == 0) return;

    std::vector<BYTE> rawData(dwSize);
    if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, rawData.data(), &dwSize, sizeof(RAWINPUTHEADER)) != dwSize) {
        std::cerr << "GetRawInputData does not return correct size!" << std::endl;
        return;
    }

    RAWINPUT* raw = (RAWINPUT*)rawData.data();
    EngineUUID deviceId = GetDeviceUUID(raw->header.hDevice);

    InputSystem* inputSystem = engine->GetSystem<InputSystem>();
    //for keyboard...
    if (raw->header.dwType == RIM_TYPEKEYBOARD) {
        RAWKEYBOARD& rawKeyboard = raw->data.keyboard;
        KeyCode key = Win32VirtualKeyToKeyCode(rawKeyboard.VKey);
        if (key == KeyCode::None) {
            //it's a keycode we don't care about.
            return;
        }

        InputState state = (rawKeyboard.Flags & RI_KEY_BREAK) ? InputState::End : InputState::Begin;
        
        inputSystem->ProcessInput(deviceId, key, state);
    } else if (raw->header.dwType == RIM_TYPEMOUSE) {
        RAWMOUSE& rawMouse = raw->data.mouse;

        //Handle mouse buttons
        struct MouseButtonInfo {
            USHORT flag;
            KeyCode key;
        };
        MouseButtonInfo buttonInfos[] = {
            { RI_MOUSE_BUTTON_1_DOWN, KeyCode::MouseButton1 },
            { RI_MOUSE_BUTTON_1_UP, KeyCode::MouseButton1 },
            { RI_MOUSE_BUTTON_2_DOWN, KeyCode::MouseButton2 },
            { RI_MOUSE_BUTTON_2_UP, KeyCode::MouseButton2 },
            { RI_MOUSE_BUTTON_3_DOWN, KeyCode::MouseButton3 },
            { RI_MOUSE_BUTTON_3_UP, KeyCode::MouseButton3 },
            { RI_MOUSE_BUTTON_4_DOWN, KeyCode::MouseButton4 },
            { RI_MOUSE_BUTTON_4_UP, KeyCode::MouseButton4 },
            { RI_MOUSE_BUTTON_5_DOWN, KeyCode::MouseButton5 },
            { RI_MOUSE_BUTTON_5_UP, KeyCode::MouseButton5 },
        };

        for (const auto& buttonInfo : buttonInfos) {
            if (rawMouse.usButtonFlags & buttonInfo.flag) {
                InputState state = (buttonInfo.flag & 0x0001) ? InputState::Begin : InputState::End;
                inputSystem->ProcessInput(deviceId, buttonInfo.key, state);
            }
        }

        //Handle mouse movement
        if (rawMouse.lLastX != 0 || rawMouse.lLastY != 0) {
            Math::Vector3<double> delta(static_cast<double>(rawMouse.lLastX), static_cast<double>(rawMouse.lLastY), 0.0);
            inputSystem->ProcessInput(deviceId, KeyCode::MouseMove, InputState::Change, Math::Vector3<double>(), delta);
        }

        //Handle mouse wheel
        if (rawMouse.usButtonFlags & RI_MOUSE_WHEEL) {
            SHORT wheelDelta = static_cast<SHORT>(rawMouse.usButtonData);
            Math::Vector3<double> delta(0.0, 0.0, static_cast<double>(wheelDelta) / WHEEL_DELTA); // Normalize by WHEEL_DELTA
            inputSystem->ProcessInput(deviceId, KeyCode::MouseScrollWheel1, InputState::Change, Math::Vector3<double>(), delta);
        }
    }
}

EngineUUID RawInputHandler::GetDeviceUUID(HANDLE deviceHandle) {
    auto it = deviceIdMap.find(deviceHandle);
    if (it != deviceIdMap.end()) {
        return it->second;
    } else {
        EngineUUID newId = EngineUUID();
        deviceIdMap[deviceHandle] = newId;
        return newId;
    }
}