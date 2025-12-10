#pragma once

#include "UI/UILayer.h"
#include "UISurfaceLayer.generated.h"

class [[reflect()]] UISurfaceLayer : public UILayer, BaseInstance<UISurfaceLayer> {
    REFLECTION()
public:
    UISurfaceLayer(Engine* engine) : UILayer(engine) {}
};

REFLECTION_END()
