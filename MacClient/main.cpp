
#include "ReflectionRegistry.h"
#include "Core/Engine.h"
#include "Platform/PlatformWindow.h"
#include "Mac/MacPlatformWindow.h"
#include "Platform/Viewport.h"
#include "ClientShared/Console.h"
#include "ClientShared/Rendering/BgfxRenderer.h"
#include <iostream>
#include <mach/mach_time.h>

class MacTimeProvider : public ITimeProvider {
public:
    MacTimeProvider() {
        mach_timebase_info_data_t timebase;
        mach_timebase_info(&timebase);
        timebaseConversion = static_cast<double>(timebase.numer) / static_cast<double>(timebase.denom) / 1e9;
    }

    double GetTimeSeconds() const override {
        uint64_t currentTime = mach_absolute_time();
        return static_cast<double>(currentTime) * timebaseConversion;
    }

private:
    double timebaseConversion;
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
    
    MacPlatformWindow window(&engine);
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
