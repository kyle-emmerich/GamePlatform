#include "ClientShared/Rendering/BgfxRenderer.h"
#include "Platform/Viewport.h"
#include "Platform/PlatformWindow.h"
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/bx.h>

namespace ClientShared {

    bgfx::VertexLayout BgfxRenderer::SimpleVertex::static_layout;

    BgfxRenderer::BgfxRenderer() {
    }

    BgfxRenderer::~BgfxRenderer() {
        bgfx::shutdown();
    }

    void BgfxRenderer::Initialize(Viewport* mainViewport) {
        if (!mainViewport->GetAttachedWindow()) {
            throw std::runtime_error("BgfxRenderer requires the first viewport to be attached to a window.");
        }

        bgfx::Init init;
#ifdef BX_PLATFORM_WINDOWS
        init.platformData.nwh = mainViewport->GetAttachedWindow()->GetNativeWindowHandle();
#elif BX_PLATFORM_LINUX || BX_PLATFORM_BSD
        //get x11 window handle
#elif BX_PLATFORM_OSX
        //get cocoa window handle
        init.platformData.nwh = (void*)mainViewport->GetAttachedWindow()->GetNativeWindowHandle();
#endif

        Math::Rect<int> windowInternalBounds = mainViewport->GetAttachedWindow()->GetInternalBounds();
        init.resolution.width = windowInternalBounds.Size().X;
        init.resolution.height = windowInternalBounds.Size().Y;
        init.resolution.reset = BGFX_RESET_VSYNC;

        if (!bgfx::init(init)) {
            throw std::runtime_error("Failed to initialize bgfx");
        }
        initialized = true;
        bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0);
        bgfx::setViewRect(0, 0, 0, bgfx::BackbufferRatio::Equal);

        //we'll also take this opportunity to initialize any shared resources.
        //todo...
    }

    void BgfxRenderer::InitializeAdditionalViewport(Viewport* viewport) {
        // For additional viewports, we just need to set up their view rects.
        if (!viewport->GetAttachedWindow()) {
            throw std::runtime_error("BgfxRenderer requires additional viewports to be attached to a window.");
        }

        //we need to make a frame buffer for this viewport.

        Math::Rect<int> windowInternalBounds = viewport->GetAttachedWindow()->GetInternalBounds();
        Math::Vector2<int> size = windowInternalBounds.Size();

        bgfx::FrameBufferHandle handle = bgfx::createFrameBuffer(size.X, size.Y, bgfx::TextureFormat::BGRA8, BGFX_TEXTURE_RT);
        viewportFrameBuffers[viewport] = handle;
        bgfx::setViewFrameBuffer(viewport->GetViewId(), handle);
        bgfx::setViewClear(viewport->GetViewId(), BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0);
        bgfx::setViewRect(viewport->GetViewId(), 0, 0, bgfx::BackbufferRatio::Equal);

    }

    void BgfxRenderer::ShutdownAdditionalViewport(Viewport* viewport) {
        auto it = viewportFrameBuffers.find(viewport);
        if (it != viewportFrameBuffers.end()) {
            bgfx::destroy(it->second);
            viewportFrameBuffers.erase(it);
        }
    }

    void BgfxRenderer::BeginFrame() {

    }

    void BgfxRenderer::EndFrame() {
        bgfx::frame();
    }

    bool BgfxRenderer::Initialized() {
        return initialized;
    }

    void BgfxRenderer::Shutdown() {
        if (initialized) {
            initialized = false;
            bgfx::shutdown();
        }
    }

    void BgfxRenderer::DrawSolidRect(Viewport* viewport, const Math::Rect<float>& rect, const Math::Color& color) {
        int viewId = viewport->GetViewId();
        
    }

}
