#pragma once
#include "Instance.h"
#include "System.generated.h"

class SystemInitOrder;

class [[reflect()]] System : public Instance, BaseInstance<System> {
	REFLECTION()
public:
	System(Engine* engine);

	void Register(SystemInitOrder& initOrder);
	void Initialize();
	void Shutdown();

	virtual void Update(double deltaTime);
};

REFLECTION_END()