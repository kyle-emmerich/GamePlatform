#include "UI/UILayer.h"
#include "UI/UIBase.h"
#include "Platform/Viewport.h"

void UILayer::OnRendered(Viewport* viewport) {
    if (!Visible) {
        return;
    }

    //todo: calculate transform of layer, apply to all child UI
    Math::Transform<double> layerTransform = GetLayerTransform();

    for (Instance* child : Children) {
        if (child->IsA<UIBase>()) {
            //render
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
