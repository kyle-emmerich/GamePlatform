#include "ClientShared/Viewport.h"
#include "ClientShared/PlatformWindow.h"
#include "bgfx/bgfx.h"
#include "bx/bx.h"
#include "bgfx/platform.h"
#include <stdexcept>
#include <iostream>

Viewport::Viewport() {
    viewId = nextViewId++;
}

Viewport::~Viewport() {
    if (windowResizedListener) {
        windowResizedListener->Disconnect();
        windowResizedListener = nullptr;
    }
    if (attachedWindow) {
        bgfx::shutdown();
    }
    attachedWindow = nullptr;
}

void Viewport::AttachToWindow(PlatformWindow* window) {
    attachedWindow = window;

    Rect<int> windowInternalBounds = attachedWindow->GetInternalBounds();
    
    bgfx::Init init;
#ifdef BX_PLATFORM_WINDOWS
    init.platformData.nwh = (void*)(uintptr_t)attachedWindow->GetNativeWindowHandle();
#elif BX_PLATFORM_LINUX || BX_PLATFORM_BSD
    //get x11 window handle
#elif BX_PLATFORM_OSX
    //get cocoa window handle
    init.platformData.nwh = (void*)attachedWindow->GetNativeWindowHandle();
#endif

    init.resolution.width = windowInternalBounds.Size().X;
    init.resolution.height = windowInternalBounds.Size().Y;
    init.resolution.reset = BGFX_RESET_VSYNC;

    if (!bgfx::init(init)) {
        throw std::runtime_error("Failed to initialize bgfx");
    }

    bgfx::setViewClear(viewId, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0);
    bgfx::setViewRect(viewId, 0, 0, bgfx::BackbufferRatio::Equal);

    windowResizedListener = &attachedWindow->Resized.Connect([this]() {
        Rect<int> newBounds = attachedWindow->GetInternalBounds();
        bgfx::reset(newBounds.Size().X, newBounds.Size().Y, BGFX_RESET_VSYNC);
        bgfx::setViewRect(viewId, 0, 0, bgfx::BackbufferRatio::Equal);

        //std::cout << "Viewport resized to " << newBounds.Size().X << "x" << newBounds.Size().Y << std::endl;
    });
}

void Viewport::RenderFrame() {
    bgfx::touch(viewId);
    for (IRenderable* renderable : renderables) {
        renderable->OnRendered(this);
    }
    bgfx::frame();
}

void Viewport::AttachRenderable(IRenderable* renderable) {
    renderables.push_back(renderable);
    renderable->OnAttached(this);
}

void Viewport::DetachRenderable(IRenderable* renderable) {
    renderable->OnDetached(this);
    renderables.erase(std::remove(renderables.begin(), renderables.end(), renderable), renderables.end());
}

int Viewport::nextViewId = 0;