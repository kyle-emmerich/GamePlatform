#include "UI/UIFrame.h"
#include "Core/Engine.h"
#include "Platform/Viewport.h"
#include "Rendering/IRenderer.h"

//todo: determine if this works when rendering to a texture?
void UIFrame::OnRender(const Math::Transform<double>& layerTransform, Viewport* viewport) {
    if (!Visible) return;

    Rendering::IRenderer* renderer = viewport->GetEngine()->GetRenderer();

    // Render background
     if (BackgroundColor.A > 0.0) {
         Math::Rect<float> absBounds = GetAbsoluteBounds();
         renderer->DrawSolidRect(viewport, absBounds, BackgroundColor);
     }

    // bool didClip = false;
    // if (ClipsDescendants) {
    //     didClip = true;
    //     renderer->PushClipRect(GetAbsoluteBounds());
    // }
    // // Render children
    // for (Instance* child : Children) {
    //     if (child->IsA<UIBase>()) {
    //         static_cast<UIBase*>(child)->OnRender(layerTransform, renderer);
    //     }
    // }
    // if (didClip) {
    //     renderer->PopClipRect();
    // }
}