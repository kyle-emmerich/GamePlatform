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
    rid[0].dwFlags = RIDEV_NOLEGACY;
    rid[0].hwndTarget = hwnd;

    rid[1].usUsagePage = 0x01; // Generic Desktop Controls
    rid[1].usUsage = 0x02;     // Mouse
    rid[1].dwFlags = RIDEV_NOLEGACY;
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