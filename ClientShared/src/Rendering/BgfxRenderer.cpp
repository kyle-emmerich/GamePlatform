#include "ClientShared/Rendering/BgfxRenderer.h"
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

namespace ClientShared {

    struct PosColorVertex
    {
        float x;
        float y;
        float z;
        uint32_t abgr;

        static void init()
        {
            ms_layout
                .begin()
                .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
                .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
                .end();
        };

        static bgfx::VertexLayout ms_layout;
    };

    bgfx::VertexLayout PosColorVertex::ms_layout;

    BgfxRenderer::BgfxRenderer() {
    }

    BgfxRenderer::~BgfxRenderer() {
        bgfx::shutdown();
    }

    bool BgfxRenderer::Init() {
        PosColorVertex::init();

        // TODO: Load shader program here
        // program = ...

        return true;
    }

    void BgfxRenderer::BeginFrame() {
        bgfx::touch(viewId);
    }

    void BgfxRenderer::EndFrame() {
        //bgfx::frame(); // Viewport handles this
    }

    void BgfxRenderer::DrawRect(const Math::Rect<float>& rect, const Math::Color& color) {
        if (bgfx::getAvailTransientVertexBuffer(4, PosColorVertex::ms_layout) == 4) {
            bgfx::TransientVertexBuffer tvb;
            bgfx::allocTransientVertexBuffer(&tvb, 4, PosColorVertex::ms_layout);

            PosColorVertex* verts = (PosColorVertex*)tvb.data;

            uint32_t abgr = color.ToABGR();

            verts[0] = { rect.left,  rect.top,    0.0f, abgr };
            verts[1] = { rect.right, rect.top,    0.0f, abgr };
            verts[2] = { rect.left,  rect.bottom, 0.0f, abgr };
            verts[3] = { rect.right, rect.bottom, 0.0f, abgr };

            bgfx::setVertexBuffer(0, &tvb);

            bgfx::TransientIndexBuffer tib;
            if (bgfx::getAvailTransientIndexBuffer(6) == 6) {
                bgfx::allocTransientIndexBuffer(&tib, 6);
                uint16_t* indices = (uint16_t*)tib.data;
                indices[0] = 0; indices[1] = 1; indices[2] = 2;
                indices[3] = 1; indices[4] = 3; indices[5] = 2;
                bgfx::setIndexBuffer(&tib);

                bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A);
                
                if (bgfx::isValid(program)) {
                    bgfx::submit(viewId, program);
                } else {
                    // Fallback debug text
                    bgfx::dbgTextPrintf(static_cast<uint16_t>(rect.left / 8), static_cast<uint16_t>(rect.top / 16), 0x0f, "RECT");
                }
            }
        }
    }

    void BgfxRenderer::PushClipRect(const Math::Rect<float>& rect) {
        bgfx::setScissor(
            static_cast<uint16_t>(rect.min.X),
            static_cast<uint16_t>(rect.min.Y),
            static_cast<uint16_t>(rect.Size().X),
            static_cast<uint16_t>(rect.Size().Y)
        );
    }

    void BgfxRenderer::PopClipRect() {
        bgfx::setScissor(0, 0, 0, 0);
    }
}
