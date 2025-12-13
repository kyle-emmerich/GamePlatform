
#include "ReflectionRegistry.h"
#include "Core/Engine.h"
#include "Windows/WindowsPlatformWindow.h"
#include "Windows/WindowsFileSystemWatcher.h"
#include "Platform/Viewport.h"
#include "ClientShared/Console.h"
#include "ClientShared/Rendering/BgfxRenderer.h"
#include <iostream>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "UI/UISystem.h"
#include "UI/UIScreenLayer.h"
#include "UI/UIFrame.h"

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
    //Very first thing to do is init the renderer's static resources.
    ClientShared::BgfxRenderer::StaticInit();

    WindowsTimeProvider* timeProvider = new WindowsTimeProvider();
    WindowsFileSystemWatcher* fileSystemWatcher = new WindowsFileSystemWatcher();

    DirectorySubscription* subscription = fileSystemWatcher->Subscribe(".");
    subscription->FileCreated.Connect([](const std::string& filename) {
        std::cout << "File created: " << filename << std::endl;
    });
    subscription->FileChanged.Connect([](const std::string& filename) {
        std::cout << "File changed: " << filename << std::endl;
    });
    subscription->FileDeleted.Connect([](const std::string& filename) {
        std::cout << "File deleted: " << filename << std::endl;
    });

    Engine* engine = new Engine;
    std::shared_ptr<ClientShared::BgfxRenderer> renderer = std::make_shared<ClientShared::BgfxRenderer>();
    EngineInitParams params;
    params.timeProvider = timeProvider;
    params.fileSystemWatcher = fileSystemWatcher;
    params.renderer = renderer.get();
    engine->Initialize(params);

    Console console(engine);
    console.TextEntered.Connect([&engine](const std::string& text) {
        engine->ExecuteConsoleLua(text);
    });
    
    WindowsPlatformWindow window(engine);
    if (!window.Initialize("Test Window", 1280, 720)) {
        std::cerr << "Failed to initialize window" << std::endl;
        return 1;
    }

    std::shared_ptr<Viewport> viewport = std::make_shared<Viewport>(engine);
    viewport->AttachToWindow(&window);
    viewport->AttachRenderable(&console);

    // //let's add some test UI and see if it renders.
    UISystem* uiSystem = engine->GetSystem<UISystem>();
    
    UIScreenLayer* testLayer = new UIScreenLayer(engine);
    testLayer->SetName("TestLayer");

    UIFrame* testFrame = new UIFrame(engine);
    testFrame->SetPosition(Math::UDim2<float>(0.1f, 0.0f, 0.1f, 0.0f));
    testFrame->SetSize(Math::UDim2<float>(0.3f, 0.0f, 0.3f, 0.0f));
    testFrame->SetBackgroundColor(Math::Color(0.0f, 1.0f, 0.0f, 1.0f));
    testFrame->SetParent(testLayer);
    testLayer->SetParent(uiSystem);
    

    std::cout << "Engine Initialized." << std::endl;

    while (window.PollEvents()) {
        fileSystemWatcher->Update();
        engine->Update();
        //pull input
        //execute some lua
        //run physics
        //execute some more lua
        renderer->BeginFrame();

        testLayer->OnRendered(viewport.get());

        viewport->RenderFrame();
        
        renderer->EndFrame();
        //execute more lua
    }

    viewport.reset();
    renderer.reset();
    
    engine->Shutdown();
    std::cout << "Engine shutdown." << std::endl;
    delete engine;
    delete timeProvider;
    delete fileSystemWatcher;
    return 0;
}
