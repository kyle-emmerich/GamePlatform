#pragma once

#include "UI/UIFrame.h"
#include "UIButton.generated.h"

class [[reflect()]] UIButton : public UIFrame, BaseInstance<UIButton> {
    REFLECTION()
public:
    UIButton(Engine* engine) : UIFrame(engine) {}

    virtual InputResult HandleInputBegin(Input* input) override;
    virtual InputResult HandleInputChange(Input* input) override;
    virtual InputResult HandleInputEnd(Input* input) override;
};

REFLECTION_END()