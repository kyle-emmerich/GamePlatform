#pragma once
#include "Platform/PlatformWindow.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

class RawInputHandler;

class WindowsPlatformWindow : public PlatformWindow, public BaseInstance<WindowsPlatformWindow> {
public:
    WindowsPlatformWindow(Engine* engine);
    virtual ~WindowsPlatformWindow();

    bool Initialize(const std::string& title, int width, int height) override;
    void Shutdown() override;
    bool PollEvents() override;
    void* GetNativeWindowHandle() const override;
    Math::Rect<int> GetInternalBounds() const override;
    Math::Rect<int> GetExternalBounds() const override;

    void ProcessRawInput(LPARAM lParam);

private:
    RawInputHandler* rawInputHandler = nullptr;

};
