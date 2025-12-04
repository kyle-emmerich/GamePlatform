#pragma once

#include "UI/LayerCollector.h"
#include "UIScreen.generated.h"

class [[reflect()]] UIScreen : public LayerCollector, BaseInstance<UIScreen> {
    REFLECTION()
public:
    UIScreen(Engine* engine) : LayerCollector(engine) {}
};

REFLECTION_END()
