#pragma once

#include "UI/UIBase.h"
#include "Math/Color.h"
#include "UIFrame.generated.h"

class UIBase;

class [[reflect()]] UIFrame : public UIBase, BaseInstance<UIFrame> {
    REFLECTION()
public:
    UIFrame(Engine* engine) : UIBase(engine) {}

    [[reflect()]]
    [[summary("Determines the background color and opacity of the UI frame.")]]
    Color BackgroundColor;

    void OnRender(Rendering::IRenderer* renderer) override;

};

REFLECTION_END()