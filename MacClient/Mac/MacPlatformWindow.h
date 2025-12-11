#pragma once

#include "Platform/PlatformWindow.h"
#include "Math/Rect.h"

class MacPlatformWindow : public PlatformWindow {
public:
    MacPlatformWindow(Engine* engine);
    ~MacPlatformWindow() override;

    bool Initialize(const std::string& title, int width, int height) override;
    void Shutdown() override;
    
    bool PollEvents() override;

    void* GetNativeWindowHandle() const override;

    Math::Rect<int> GetInternalBounds() const override;
    Math::Rect<int> GetExternalBounds() const override;

private:
    void* nsWindow = nullptr;
    void* nsView = nullptr;
    bool shouldClose = false;
};
