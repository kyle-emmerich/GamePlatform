#pragma once

#include "Input/Input.h"

class MacInputHandler {
public:
    MacInputHandler();
    ~MacInputHandler();

    void Initialize();
    void Shutdown();
    void Update();
};
