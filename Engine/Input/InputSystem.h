#pragma once

#include "Instance/System.h"
#include "Input/Input.h"
#include "Input/IInputFocusable.h"
#include "Core/Event.h"
#include "InputSystem.generated.h"

struct InputDevice {
	std::unordered_map<KeyCode, Input*> inputsByKeyCode;
};

class [[reflect()]] InputSystem : public System, BaseInstance<InputSystem> {
	REFLECTION()
public:
	InputSystem(Engine* engine) : System(engine) {}

	[[reflect()]]
	[[summary("Fires when an input begins.")]]
	MulticastEvent<Input*> InputBegan;
	[[reflect()]]
	[[summary("Fires when an input changes.")]]
	MulticastEvent<Input*> InputChanged;
	[[reflect()]]
	[[summary("Fires when an input ends.")]]
	MulticastEvent<Input*> InputEnded;

	[[reflect()]]
	[[summary("Fires when text is entered.")]]
	MulticastEvent<std::string> TextEntered;

	[[reflect()]]
	IInputFocusable* FocusedInstance = nullptr;

	[[reflect()]]
	[[summary("Sets focus to the given input focusable instance.")]]
	void Focus(IInputFocusable* instance, Input* instigatingInput = nullptr);

	[[reflect()]]
	[[summary("Clears focus from the current input focusable instance.")]]
	void ClearFocus(Input* instigatingInput = nullptr);

	[[reflect()]]
	[[summary("Returns true if the given key is currently held down, false otherwise.")]]
	bool IsKeyDown(KeyCode key) const;

	void ProcessInput(EngineUUID deviceId, KeyCode key, InputState state, Vector3<double> position = Vector3<double>(), Vector3<double> delta = Vector3<double>());
	void ProcessTextInput(const std::string& text);

protected:
	std::unordered_map<EngineUUID, InputDevice> inputDevices;

	InputDevice& getInputDevice(EngineUUID deviceId);
	Input* getInput(EngineUUID deviceId, KeyCode key);
};

REFLECTION_END()