#pragma once

#include <string>
#include <vector>
#include "Rendering/IRenderable.h"
#include "Core/Export.h"
#include "Core/Event.h"

class PlatformWindow;

class GP_EXPORT Viewport {
public:
    Viewport();
    virtual ~Viewport();

    void AttachToWindow(PlatformWindow* window);

    void AttachRenderable(IRenderable* renderable);
    void DetachRenderable(IRenderable* renderable);
    
    void RenderFrame();

    PlatformWindow* GetAttachedWindow() const { return attachedWindow; }
    int GetViewId() const { return viewId; }
protected:
    PlatformWindow* attachedWindow = nullptr;

    int viewId;
    static int nextViewId;

    std::vector<IRenderable*> renderables;

    MulticastEvent<>::Listener* windowResizedListener = nullptr;
};
