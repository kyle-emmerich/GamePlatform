#pragma once
#include "Physics/Constraints/ConstraintBase.h"
#include "HingeConstraint.generated.h"

class [[reflect()]] HingeConstraint : public ConstraintBase, BaseInstance<HingeConstraint> {
    REFLECTION()
public:
    HingeConstraint(Engine* engine) : ConstraintBase(engine) {}

    
};

REFLECTION_END()