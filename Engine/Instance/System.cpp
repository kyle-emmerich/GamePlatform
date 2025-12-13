#include "Instance/System.h"
#include "Core/Engine.h"

System::System(Engine* engine) : Instance(engine) {}

void System::Register(SystemInitOrder& initOrder) {
    initOrder.Add(this);
}

void System::Initialize() {}
void System::Shutdown() {}
void System::Update(double deltaTime) {}
