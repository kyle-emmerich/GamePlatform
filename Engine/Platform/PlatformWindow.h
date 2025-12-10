#pragma once

#include <string>
#include <cstdint>
#include "Math/Rect.h"
#include "Core/Event.h"
#include "Input/KeyCode.h"
#include "Input/Input.h"

#include "Core/Export.h"
#include "Instance/Instance.h"

#include "PlatformWindow.generated.h"

class Engine;

class GP_EXPORT [[reflect(Abstract)]] PlatformWindow : public Instance, BaseInstance<PlatformWindow> {
    REFLECTION()
public:
    PlatformWindow(Engine* engine);
    virtual ~PlatformWindow();

    virtual bool Initialize(const std::string& title, int width, int height) = 0;
    virtual void Shutdown() = 0;
    
    // Process OS messages (returns false if quit requested)
    virtual bool PollEvents() = 0;

    virtual void* GetNativeWindowHandle() const = 0;

    virtual Math::Rect<int> GetInternalBounds() const = 0;
    virtual Math::Rect<int> GetExternalBounds() const = 0;

    [[reflect()]]
    MulticastEvent<> Resized;
    [[reflect()]]
    MulticastEvent<> Closed;
    [[reflect()]]
    MulticastEvent<> Focused;
    [[reflect()]]
    MulticastEvent<> Unfocused;

    [[reflect()]]
    MulticastEvent<EngineUUID, KeyCode, InputState> KeyInputReceived;
    [[reflect()]]
    MulticastEvent<std::string> ReceivedTextInput;

protected:
    void* windowHandle = nullptr;
};

REFLECTION_END()