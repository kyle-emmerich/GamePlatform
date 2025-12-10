#pragma once

#include "UI/UILayer.h"
#include "UIScreenLayer.generated.h"

class [[reflect()]] UIScreenLayer : public UILayer, BaseInstance<UIScreenLayer> {
    REFLECTION()
public:
    UIScreenLayer(Engine* engine) : UILayer(engine) {}

    Math::Transform<double> GetLayerTransform();
};

REFLECTION_END()
