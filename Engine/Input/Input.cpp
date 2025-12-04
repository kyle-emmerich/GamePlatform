#include "Input/Input.h"

void Input::SetPosition(const Vector3<double>& position) {
    if (Position == position) {
        return;
    }
    Position = position;
    this->raisePropChanged(prop_Position);
}

void Input::SetDelta(const Vector3<double>& delta) {
    if (Delta == delta) {
        return;
    }
    Delta = delta;
    this->raisePropChanged(prop_Delta);
}

void Input::SetState(InputState state) {
    if (State == state) {
        return;
    }
    State = state;
    this->raisePropChanged(prop_State);
}

