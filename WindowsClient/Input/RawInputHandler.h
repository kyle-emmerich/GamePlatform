#pragma once

#include "ClientShared/PlatformWindow.h"
#include "Windows/WindowsPlatformWindow.h"
#include "Instance/UUID.h"

#include <unordered_map>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

class Engine;
class RawInputHandler {
public:
    RawInputHandler(Engine* engine);
    ~RawInputHandler();

    void Initialize(PlatformWindow* _window);
    void Shutdown();

    void HandleInput(LPARAM lParam);
private:
    Engine* engine;
    PlatformWindow* window = nullptr;

    //Get or create a UUID for the given device handle
    EngineUUID GetDeviceUUID(HANDLE deviceHandle);
    std::unordered_map<HANDLE, EngineUUID> deviceIdMap;
};