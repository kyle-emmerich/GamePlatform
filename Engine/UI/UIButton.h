#pragma once

#include "UI/UIFrame.h"
#include "UIButton.generated.h"

class [[reflect()]] UIButton : public UIFrame, BaseInstance<UIButton> {
    REFLECTION()
public:
    UIButton(Engine* engine) : UIFrame(engine) {}
};

REFLECTION_END()