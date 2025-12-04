#pragma once
#include "Rendering/IRenderer.h"
#include "ClientShared/Export.h"
#include <bgfx/bgfx.h>

#include "Math/Rect.h"
#include "Math/Color.h"

namespace ClientShared {

    class CLIENT_API BgfxRenderer : public Rendering::IRenderer {
    public:
        BgfxRenderer();
        virtual ~BgfxRenderer();

        bool Init();
        void SetViewId(int viewId) { this->viewId = viewId; }

        void BeginFrame() override;
        void EndFrame() override;

        void DrawRect(const Rect<float>& rect, const Color& color) override;
        void PushClipRect(const Rect<float>& rect) override;
        void PopClipRect() override;

    private:
        bgfx::ProgramHandle program = BGFX_INVALID_HANDLE;
        int viewId = 0;
    };

}
