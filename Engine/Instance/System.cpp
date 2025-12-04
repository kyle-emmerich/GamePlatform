#include "Instance/System.h"
#include "Core/Engine.h"

System::System(Engine* engine) : Instance(engine) {}

void System::Initialize() {}
void System::Shutdown() {}

void System::Register(SystemInitOrder& initOrder) {
	//By default, just add to the end of the init order.
	initOrder.Add(this);
}

void System::Update(double deltaTime) {
	//Default implementation does nothing.
}