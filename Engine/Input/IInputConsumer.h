#pragma once

class Input;

enum class InputResult {
    NotHandled,
    Handled
};

class IInputConsumer {
public:
    virtual ~IInputConsumer() = default;

    virtual InputResult HandleInputBegin(Input* input) = 0;
    virtual InputResult HandleInputChange(Input* input) = 0;
    virtual InputResult HandleInputEnd(Input* input) = 0;
};