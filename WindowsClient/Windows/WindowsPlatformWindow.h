#pragma once
#include "Platform/PlatformWindow.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

class RawInputHandler;

class WindowsPlatformWindow : public PlatformWindow {
public:
    WindowsPlatformWindow(Engine* engine);
    virtual ~WindowsPlatformWindow();

    bool Initialize(const std::string& title, int width, int height) override;
    void Shutdown() override;
    bool PollEvents() override;
    void* GetNativeWindowHandle() const override;
    Rect<int> GetInternalBounds() const override;
    Rect<int> GetExternalBounds() const override;

    void ProcessRawInput(LPARAM lParam);

private:
    RawInputHandler* rawInputHandler = nullptr;

};
