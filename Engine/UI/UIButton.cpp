#include "UI/UIButton.h"

InputResult UIButton::HandleInputBegin(Input* input) {
    // Check if the button is visible and enabled
    if (!Visible || !Enabled) {
        return InputResult::NotHandled;
    }

    // Check if the input position is within the button's bounds
    Math::Vector2<float> inputPos = input->GetPosition();
    Math::Rect<float> buttonBounds = GetAbsoluteBounds();
    if (buttonBounds.Contains(inputPos)) {
        // Handle button press logic here (e.g., change appearance)
        // ...

        return InputResult::Handled;
    }

    return InputResult::NotHandled;
}

InputResult UIButton::HandleInputChange(Input* input) {
    // Check if the button is visible and enabled
    if (!Visible || !Enabled) {
        return InputResult::NotHandled;
    }

    // Check if the input position is within the button's bounds
    Math::Vector2<float> inputPos = input->GetPosition();
    Math::Rect<float> buttonBounds = GetAbsoluteBounds();
    if (buttonBounds.Contains(inputPos)) {
        // Handle button hover logic here (e.g., change appearance)
        // ...

        return InputResult::Handled;
    }

    return InputResult::NotHandled;
}

InputResult UIButton::HandleInputEnd(Input* input) {
    // Check if the button is visible and enabled
    if (!Visible || !Enabled) {
        return InputResult::NotHandled;
    }

    // Check if the input position is within the button's bounds
    Math::Vector2<float> inputPos = input->GetPosition();
    Math::Rect<float> buttonBounds = GetAbsoluteBounds();
    if (buttonBounds.Contains(inputPos)) {
        // Handle button release logic here (e.g., trigger click event)
        // ...

        return InputResult::Handled;
    }

    return InputResult::NotHandled;
}