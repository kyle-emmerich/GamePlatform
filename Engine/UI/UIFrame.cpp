#include "UI/UIFrame.h"

//todo: determine if this works when rendering to a texture?
void UIFrame::OnRender(Rendering::IRenderer* renderer) {
    if (!Visible) return;

    // Render background
    if (BackgroundColor.A > 0.0) {
        Math::Rect<float> absBounds = GetAbsoluteBounds();
        renderer->DrawRect(absBounds, BackgroundColor);
    }

    bool didClip = false;
    if (ClipsDescendants) {
        didClip = true;
        renderer->PushClipRect(GetAbsoluteBounds());
    }
    // Render children
    for (Instance* child : Children) {
        if (child->IsA<UIBase>()) {
            static_cast<UIBase*>(child)->OnRender(renderer);
        }
    }
    if (didClip) {
        renderer->PopClipRect();
    }
}