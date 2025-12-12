#pragma once

#include "UI/UIBase.h"
#include "Math/Color.h"
#include "Math/Transform.h"
#include "UIFrame.generated.h"

class Viewport;

class [[reflect()]] UIFrame : public UIBase, BaseInstance<UIFrame> {
    REFLECTION()
public:
    UIFrame(Engine* engine) : UIBase(engine) {}

    [[reflect()]]
    [[summary("Determines the background color and opacity of the UI frame.")]]
    Math::Color BackgroundColor;

    void OnRender(const Math::Transform<double>& layerTransform, Viewport* viewport);

};

REFLECTION_END()