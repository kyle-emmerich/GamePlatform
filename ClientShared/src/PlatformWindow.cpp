#include "ClientShared/PlatformWindow.h"
#include "Core/Engine.h"
#include "Input/InputSystem.h"

PlatformWindow::PlatformWindow(Engine* engine) : engine(engine) {
    KeyInputReceived.Connect([this](EngineUUID deviceId, KeyCode key, InputState state) {
        //pass to inputsystem
        this->engine->GetSystem<InputSystem>()->ProcessInput(deviceId, key, state);
    });
    ReceivedTextInput.Connect([this](const std::string& text) {
        //pass to inputsystem
        this->engine->GetSystem<InputSystem>()->ProcessTextInput(text);
    });

}

PlatformWindow::~PlatformWindow() {
}


