#pragma once
#include "Rendering/IRenderer.h"
#include "ClientShared/Export.h"
#include <bgfx/bgfx.h>

#include "Math/Rect.h"
#include "Math/Color.h"
#include "ClientShared/Rendering/Shader.h"

#include <unordered_map>
#include <memory>

class Viewport;

namespace ClientShared {

    class CLIENT_API BgfxRenderer : public Rendering::IRenderer {
    public:
        BgfxRenderer();
        virtual ~BgfxRenderer();

        virtual void Initialize(Viewport* mainViewport) override;
        void Shutdown() override;
        bool Initialized() override;
        
        void BeginFrame() override;
        void EndFrame() override;

        void InitializeAdditionalViewport(Viewport* viewport);
        void ShutdownAdditionalViewport(Viewport* viewport);

        void OnViewportResized(Viewport* viewport, const Math::Vector2<int>& newSize) override;

        void DrawSolidRect(Viewport* viewport, const Math::Rect<float>& rect, const Math::Color& color) override;
    private:
        Viewport* mainViewport = nullptr;
        std::unordered_map<Viewport*, bgfx::FrameBufferHandle> viewportFrameBuffers;

        bool initialized = false;

        struct SimpleVertex {
            float x, y, z;
            uint32_t abgr;

            static void init() {
                static_layout
                    .begin()
                    .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
                    .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
                    .end();
            }
            static bgfx::VertexLayout static_layout;
        };

        bgfx::VertexBufferHandle solidRectVbh = BGFX_INVALID_HANDLE;
        bgfx::IndexBufferHandle solidRectIbh = BGFX_INVALID_HANDLE;

        std::shared_ptr<Shader> solidRectShader;
    public:
        static void StaticInit() {
            SimpleVertex::init();
        }
    };

}
