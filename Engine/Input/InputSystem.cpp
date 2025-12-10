#include "Input/InputSystem.h"
#include "Input/IInputFocusable.h"
#include "Input/Input.h"

void InputSystem::Focus(IInputFocusable* focusable, Input* instigatingInput) {
    if (FocusedInstance) {
        FocusedInstance->__SetFocused(false, instigatingInput);
    }
    FocusedInstance = focusable;
    if (FocusedInstance) {
        FocusedInstance->__SetFocused(true, instigatingInput);
    }
}

void InputSystem::ClearFocus(Input* instigatingInput) {
    if (FocusedInstance) {
        FocusedInstance->__SetFocused(false, instigatingInput);
    }
    FocusedInstance = nullptr;
}

void InputSystem::ProcessInput(EngineUUID deviceId, KeyCode key, InputState state, Math::Vector3<double> position, Math::Vector3<double> delta) {
    Input* input = getInput(deviceId, key);
    input->SetState(state);
    input->SetPosition(position);
    input->SetDelta(delta);

    switch (state) {
        case InputState::Begin:
            InputBegan.Fire(input);
            break;
        case InputState::Change:
            InputChanged.Fire(input);
            break;
        case InputState::End:
            InputEnded.Fire(input);
            break;
    }
}

void InputSystem::ProcessTextInput(const std::string& text) {
    TextEntered.Fire(text);
}

bool InputSystem::IsKeyDown(KeyCode key) const {
    auto deviceIt = inputDevices.find(EngineUUID()); // Use default device for now
    if (deviceIt != inputDevices.end()) {
        const InputDevice& device = deviceIt->second;
        auto inputIt = device.inputsByKeyCode.find(key);
        if (inputIt != device.inputsByKeyCode.end()) {
            Input* input = inputIt->second;
            return input->State != InputState::End;
        }
    }
    return false;
}

InputDevice& InputSystem::getInputDevice(EngineUUID deviceId) {
    auto it = inputDevices.find(deviceId);
    if (it != inputDevices.end()) {
        return it->second;
    } else {
        InputDevice newDevice;
        inputDevices[deviceId] = newDevice;
        return inputDevices[deviceId];
    }
}

Input* InputSystem::getInput(EngineUUID deviceId, KeyCode key) {
    InputDevice& device = getInputDevice(deviceId);
    auto it = device.inputsByKeyCode.find(key);
    if (it != device.inputsByKeyCode.end()) {
        return it->second;
    } else {
        Input* newInput = new Input(GetEngine());
        newInput->DeviceId = deviceId;
        newInput->Key = key;
        newInput->Type = InputType::Keyboard; // Assume keyboard for now
        newInput->State = InputState::End; // Default state
        device.inputsByKeyCode[key] = newInput;
        return newInput;
    }
}