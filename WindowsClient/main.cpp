
#include "ReflectionRegistry.h"
#include "Core/Engine.h"
#include "ClientShared/PlatformWindow.h"
#include "Windows/WindowsPlatformWindow.h"
#include "ClientShared/Viewport.h"
#include "ClientShared/Console.h"
#include "ClientShared/Rendering/BgfxRenderer.h"
#include <iostream>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

class WindowsTimeProvider : public ITimeProvider {
public:
    double GetTimeSeconds() const override {
        static LARGE_INTEGER frequency;
        static bool frequencyInitialized = false;
        if (!frequencyInitialized) {
            QueryPerformanceFrequency(&frequency);
            frequencyInitialized = true;
        }

        LARGE_INTEGER currentTime;
        QueryPerformanceCounter(&currentTime);
        return static_cast<double>(currentTime.QuadPart) / static_cast<double>(frequency.QuadPart);
    }
};

int main(int argc, char** argv) {    
    Engine engine;
    ClientShared::BgfxRenderer renderer;
    EngineInitParams params;
    params.renderer = &renderer;
    engine.Initialize(params);

    Console console(&engine);
    console.TextEntered.Connect([&engine](const std::string& text) {
        engine.ExecuteConsoleLua(text);
    });
    
    WindowsPlatformWindow window(&engine);
    if (!window.Initialize("Test Window", 1280, 720)) {
        std::cerr << "Failed to initialize window" << std::endl;
        return 1;
    }

    Viewport viewport;
    viewport.AttachToWindow(&window);
    viewport.AttachRenderable(&console);

    std::cout << "Engine Initialized." << std::endl;

    while (window.PollEvents()) {
        engine.Update();
        //pull input
        //execute some lua
        //run physics
        //execute some more lua
        viewport.RenderFrame();
        //execute more lua
    }
    
    engine.Shutdown();
    std::cout << "Engine shutdown." << std::endl;
    return 0;
}
