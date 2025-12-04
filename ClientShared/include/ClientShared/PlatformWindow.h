#pragma once

#include <string>
#include <cstdint>
#include "Math/Rect.h"
#include "Core/Event.h"
#include "Input/KeyCode.h"
#include "Input/Input.h"

#include "ClientShared/Export.h"

class Engine;

class CLIENT_API PlatformWindow {
public:
    PlatformWindow(Engine* engine);
    virtual ~PlatformWindow();

    virtual bool Initialize(const std::string& title, int width, int height) = 0;
    virtual void Shutdown() = 0;
    
    // Process OS messages (returns false if quit requested)
    virtual bool PollEvents() = 0;

    virtual void* GetNativeWindowHandle() const = 0;

    virtual Rect<int> GetInternalBounds() const = 0;
    virtual Rect<int> GetExternalBounds() const = 0;

    MulticastEvent<> Resized;
    MulticastEvent<> Closed;
    MulticastEvent<> Focused;
    MulticastEvent<> Unfocused;

    MulticastEvent<EngineUUID, KeyCode, InputState> KeyInputReceived;
    MulticastEvent<std::string> ReceivedTextInput;

protected:
    Engine* engine;
    void* windowHandle = nullptr;
};
