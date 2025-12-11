#include "ClientShared/Rendering/BgfxRenderer.h"
#include "Platform/Viewport.h"
#include "Platform/PlatformWindow.h"
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/bx.h>
#include <bx/math.h>
#include "Math/Transform.h"
#include <iostream>

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
        this->mainViewport = mainViewport;
        initialized = true;
        bgfx::setViewClear(mainViewport->GetViewId(), BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0);
        bgfx::setViewRect(mainViewport->GetViewId(), 0, 0, bgfx::BackbufferRatio::Equal);

        SimpleVertex::init();
        
        // Create unit quad (0,0 to 1,1)
        const SimpleVertex vertices[] = {
            { 0.0f, 0.0f, 0.0f, 0xffffffff },
            { 1.0f, 0.0f, 0.0f, 0xffffffff },
            { 1.0f, 1.0f, 0.0f, 0xffffffff },
            { 0.0f, 1.0f, 0.0f, 0xffffffff },
        };
        const uint16_t indices[] = { 0, 1, 2, 0, 2, 3 };

        solidRectVbh = bgfx::createVertexBuffer(bgfx::copy(vertices, sizeof(vertices)), SimpleVertex::static_layout);
        solidRectIbh = bgfx::createIndexBuffer(bgfx::copy(indices, sizeof(indices)));
        
        solidRectShader = Shader::Load("ui");
        if (!solidRectShader) {
            std::cerr << "CRITICAL ERROR: Failed to load UI shader!" << std::endl;
        } else {
            std::cout << "UI Shader loaded successfully." << std::endl;
        }

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
        bgfx::resetView(viewport->GetViewId());
    }

    void BgfxRenderer::BeginFrame() {
        if (mainViewport) {
            bgfx::touch(mainViewport->GetViewId());
            
            // Set up orthographic projection for UI
            Math::Rect<int> bounds = mainViewport->GetAttachedWindow()->GetInternalBounds();
            float width = (float)bounds.Size().X;
            float height = (float)bounds.Size().Y;
            
            float ortho[16];
            bx::mtxOrtho(ortho, 0.0f, width, height, 0.0f, -1.0f, 100.0f, 0.0f, bgfx::getCaps()->homogeneousDepth);
            bgfx::setViewTransform(mainViewport->GetViewId(), NULL, ortho);
            bgfx::setViewRect(mainViewport->GetViewId(), 0, 0, (uint16_t)width, (uint16_t)height);
        }
    }

    void BgfxRenderer::EndFrame() {
        bgfx::frame();
    }

    bool BgfxRenderer::Initialized() {
        return initialized;
    }

    void BgfxRenderer::Shutdown() {
        if (initialized) {
            if (bgfx::isValid(solidRectVbh)) bgfx::destroy(solidRectVbh);
            if (bgfx::isValid(solidRectIbh)) bgfx::destroy(solidRectIbh);
            
            initialized = false;
            bgfx::shutdown();
        }
    }

    void BgfxRenderer::OnViewportResized(Viewport* viewport, const Math::Vector2<int>& newSize) {
        auto it = viewportFrameBuffers.find(viewport);
        if (it != viewportFrameBuffers.end()) {
            bgfx::destroy(it->second);
            bgfx::FrameBufferHandle handle = bgfx::createFrameBuffer(newSize.X, newSize.Y, bgfx::TextureFormat::BGRA8, BGFX_TEXTURE_RT);
            viewportFrameBuffers[viewport] = handle;
            bgfx::setViewFrameBuffer(viewport->GetViewId(), handle);
            bgfx::setViewRect(viewport->GetViewId(), 0, 0, bgfx::BackbufferRatio::Equal);
        }
    }

    void BgfxRenderer::DrawSolidRect(Viewport* viewport, const Math::Rect<float>& rect, const Math::Color& color) {
        int viewId = viewport->GetViewId();
        
        if (!solidRectShader || !solidRectShader->IsValid()) {
            std::cerr << "Solid rect shader invalid" << std::endl;
            return;
        }

        Math::Transform<float> transform;
        transform.SetTranslation(Math::Vector3<float>(rect.left, rect.top, 0.0f));
        transform.m00 = rect.right - rect.left;
        transform.m11 = rect.bottom - rect.top;

        solidRectShader->SetUniform("u_rect", transform);
        solidRectShader->SetUniform("u_rectColor", color);

        // Debug print matrix
        /*
        std::cout << "Matrix:" << std::endl;
        for(int i=0; i<4; i++) {
            for(int j=0; j<4; j++) {
                std::cout << transform.m[j][i] << " ";
            }
            std::cout << std::endl;
        }
        */

        
        bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_BLEND_ALPHA);
        bgfx::setVertexBuffer(0, solidRectVbh);
        bgfx::setIndexBuffer(solidRectIbh);
        bgfx::submit(viewId, solidRectShader->GetHandle());
    }

}
