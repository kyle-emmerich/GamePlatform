#pragma once

#include "Instance/Instance.h"
#include "LayerCollector.generated.h"
#include "Rendering/IRenderer.h"

/// @brief Responsible for rendering any UI elements that are a descendant of it.
class [[reflect()]] LayerCollector : public Instance, BaseInstance<LayerCollector> {
    REFLECTION()
public:
    LayerCollector(Engine* engine) : Instance(engine) {}

    void Render(Rendering::IRenderer* renderer);
};

REFLECTION_END()