
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
    Engine* engine = new Engine();
    std::shared_ptr<ClientShared::BgfxRenderer> renderer = std::make_shared<ClientShared::BgfxRenderer>();
    EngineInitParams params;
    params.renderer = renderer.get();
    engine->Initialize(params);

    Console console(engine);
    console.TextEntered.Connect([&engine](const std::string& text) {
        engine->ExecuteConsoleLua(text);
    });
    
    MacPlatformWindow window(engine);
    if (!window.Initialize("Test Window", 1280, 720)) {
        std::cerr << "Failed to initialize window" << std::endl;
        return 1;
    }

    std::shared_ptr<Viewport> viewport = std::make_shared<Viewport>(engine);
    viewport->AttachToWindow(&window);
    viewport->AttachRenderable(&console);

    std::cout << "Engine Initialized." << std::endl;

    while (window.PollEvents()) {
        engine->Update();
        //pull input
        //execute some lua
        //run physics
        //execute some more lua
        renderer->BeginFrame();

        renderer->DrawSolidRect(viewport.get(), Math::Rect<float>(0, 0, 500, 500), Math::Color(1, 0, 0, 1));

        viewport->RenderFrame();
        renderer->EndFrame();
        //execute more lua
    }

    viewport.reset();
    renderer.reset();
    
    engine->Shutdown();
    std::cout << "Engine shutdown." << std::endl;
    delete engine;
    return 0;
}
