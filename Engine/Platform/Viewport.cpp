#include "Platform/Viewport.h"
#include "Platform/PlatformWindow.h"
#include "bgfx/bgfx.h"
#include "bx/bx.h"
#include "bgfx/platform.h"
#include "Math/Vector2.h"
#include "Core/Engine.h"
#include <stdexcept>
#include <iostream>

Viewport::Viewport(Engine* engine) : Instance(engine) {
    viewId = nextViewId++;
}

Viewport::~Viewport() {
    engine->unregisterViewport(this);

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

    engine->registerViewport(this);

    Math::Rect<int> windowInternalBounds = attachedWindow->GetInternalBounds();
    
 


    windowResizedListener = &attachedWindow->Resized.Connect([this]() {
        Math::Rect<int> newBounds = attachedWindow->GetInternalBounds();
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

    //find all UILayers associated with this viewport and render them.
    

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

Math::Vector2<double> Viewport::GetSize() const {
    if (attachedWindow) {
        Math::Rect<int> bounds = attachedWindow->GetInternalBounds();
        return Math::Vector2<double>(static_cast<double>(bounds.Size().X), static_cast<double>(bounds.Size().Y));
    }
    return Math::Vector2<double>(0.0, 0.0);
}

int Viewport::nextViewId = 0;