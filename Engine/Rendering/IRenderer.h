#pragma once

#include "Math/Transform.h"
#include "Math/Vector2.h"
#include "Math/Color.h"

class Viewport;

namespace Rendering {

    class IRenderer {
    public:
        virtual ~IRenderer() = default;

        virtual void BeginFrame() = 0;
        virtual void EndFrame() = 0;

        virtual bool Initialized() = 0;
        virtual void Initialize(Viewport* mainViewport) = 0;
        virtual void Shutdown() = 0;

        virtual void InitializeAdditionalViewport(Viewport* viewport) = 0;
        virtual void ShutdownAdditionalViewport(Viewport* viewport) = 0;
        virtual void OnViewportResized(Viewport* viewport, const Math::Vector2<int>& newSize) = 0;


        virtual void DrawSolidRect(Viewport* viewport, const Math::Transform<float>& transform, const Math::Vector2<float>& size, const Math::Color& color) = 0;
    };

}
