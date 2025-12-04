#pragma once

#include "UI/LayerCollector.h"
#include "UISurface.generated.h"

class [[reflect()]] UISurface : public LayerCollector, BaseInstance<UISurface> {
    REFLECTION()
public:
    UISurface(Engine* engine) : LayerCollector(engine) {}
};

REFLECTION_END()
