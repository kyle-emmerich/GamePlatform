#pragma once

#include "Instance/Instance.h"
#include "ConstraintBase.generated.h"

class [[reflect()]] ConstraintBase : public Instance, BaseInstance<ConstraintBase> {
    REFLECTION()
public:
    ConstraintBase(Engine* engine) : Instance(engine) {}

    [[reflect()]]
    Attachment* Attachment0 = nullptr;
    [[reflect()]]
    Attachment* Attachment1 = nullptr;
    
};

REFLECTION_END()