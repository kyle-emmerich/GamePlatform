#pragma once

#include "Math/Rect.h"
#include "Math/Vector2.h"
#include "Math/Color.h"

namespace Rendering {

    class IRenderer {
    public:
        virtual ~IRenderer() = default;

        virtual void BeginFrame() = 0;
        virtual void EndFrame() = 0;

        virtual void DrawRect(const Math::Rect<float>& rect, const Math::Color& color) = 0;
        // Add more drawing methods as needed (DrawText, DrawTexture, etc.)
        
        // Clipping
        virtual void PushClipRect(const Math::Rect<float>& rect) = 0;
        virtual void PopClipRect() = 0;
    };

}
