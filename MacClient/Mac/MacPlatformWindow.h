#pragma once

#include "Platform/PlatformWindow.h"

class MacPlatformWindow : public PlatformWindow {
public:
    MacPlatformWindow(Engine* engine);
    ~MacPlatformWindow() override;

    bool Initialize(const std::string& title, int width, int height) override;
    void Shutdown() override;
    
    bool PollEvents() override;

    void* GetNativeWindowHandle() const override;

    Rect<int> GetInternalBounds() const override;
    Rect<int> GetExternalBounds() const override;

private:
    void* nsWindow = nullptr;
    void* nsView = nullptr;
    bool shouldClose = false;
};
