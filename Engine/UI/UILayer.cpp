#include "UI/UILayer.h"
#include "UI/UIBase.h"
#include "Platform/Viewport.h"
#include "Platform/PlatformWindow.h"
#include "Core/Engine.h"

void UILayer::OnRendered(Viewport* viewport) {
    if (!Visible) {
        return;
    }

    Math::Vector2<float> viewportSize = viewport->GetSize();
    if (viewportSize != AbsoluteSize) {
        AbsoluteSize = viewportSize;
        raisePropChanged(prop_AbsoluteSize);
    }

    //todo: calculate transform of layer, apply to all child UI
    Math::Transform<double> layerTransform = GetLayerTransform();

    for (Instance* child : Children) {
        if (child->IsA<UIBase>()) {
            //render
            UIBase* uiElement = static_cast<UIBase*>(child);
            uiElement->OnRender(layerTransform, viewport);
        }
    }
}

Math::Transform<double> UILayer::GetLayerTransform() {
    //Identity. This needs to be implemented by base classes.
    return Math::Transform<double>();
}

void UILayer::OnAttached(Viewport* viewport) {
    //Mark the viewport as needing a render.
    if (viewport) {
        viewport->MarkDirty();
    }

    
}

void UILayer::OnDetached(Viewport* viewport) {
    //Mark the viewport as needing a render.
    if (viewport) {
        viewport->MarkDirty();
    }
}

InputResult UILayer::HandleInputBegin(Input* input) {
    if (!Visible) {
        return InputResult::NotHandled;
    }

    //propagate to children
    for (Instance* child : Children) {
        if (child->IsA<UIBase>()) {
            UIBase* uiElement = static_cast<UIBase*>(child);
            InputResult result = uiElement->HandleInputBegin(input);
            if (result == InputResult::Handled) {
                return InputResult::Handled;
            }
        }
    }

    return InputResult::NotHandled;
}

InputResult UILayer::HandleInputChange(Input* input) {
    if (!Visible) {
        return InputResult::NotHandled;
    }

    //propagate to children
    for (Instance* child : Children) {
        if (child->IsA<UIBase>()) {
            UIBase* uiElement = static_cast<UIBase*>(child);
            InputResult result = uiElement->HandleInputChange(input);
            if (result == InputResult::Handled) {
                return InputResult::Handled;
            }
        }
    }

    return InputResult::NotHandled;
}

InputResult UILayer::HandleInputEnd(Input* input) {
    if (!Visible) {
        return InputResult::NotHandled;
    }

    //propagate to children
    for (Instance* child : Children) {
        if (child->IsA<UIBase>()) {
            UIBase* uiElement = static_cast<UIBase*>(child);
            InputResult result = uiElement->HandleInputEnd(input);
            if (result == InputResult::Handled) {
                return InputResult::Handled;
            }
        }
    }

    return InputResult::NotHandled;
}