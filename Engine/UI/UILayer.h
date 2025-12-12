#pragma once

#include "Instance/Instance.h"
#include "Rendering/IRenderer.h"
#include "Rendering/IRenderable.h"
#include "Math/Transform.h"
#include "UILayer.generated.h"

class Viewport;

/// @brief Responsible for rendering any UI elements that are a descendant of it.
/// UILayer is the base class for UIScreenLayer and UISurfaceLayer.
class [[reflect()]] UILayer : public Instance, BaseInstance<UILayer>, IRenderable {
    REFLECTION()
public:
    UILayer(Engine* engine) : Instance(engine) {}

    Math::Transform<double> GetLayerTransform();
    void OnRendered(Viewport* viewport) override;
    void OnAttached(Viewport* viewport) override;
    void OnDetached(Viewport* viewport) override;

    [[reflect(ReadOnly)]]
    [[summary("The absolute size in pixels of the layer.")]]
    Math::Vector2<float> AbsoluteSize;

    [[reflect()]]
    bool Visible = true;
};

REFLECTION_END()