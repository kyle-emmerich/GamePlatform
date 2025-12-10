#pragma once

#include <string>
#include <vector>
#include "Instance/Instance.h"
#include "Rendering/IRenderable.h"
#include "Core/Export.h"
#include "Core/Event.h"

#include "Math/Vector2.h"

#include "Viewport.generated.h"

class PlatformWindow;
class Engine;

class [[reflect()]] GP_EXPORT Viewport : public Instance, BaseInstance<Viewport> {
    REFLECTION()
public:
    Viewport(Engine* engine);
    virtual ~Viewport();

    void AttachToWindow(PlatformWindow* window);

    void AttachRenderable(IRenderable* renderable);
    void DetachRenderable(IRenderable* renderable);
    
    void RenderFrame();

    PlatformWindow* GetAttachedWindow() const { return attachedWindow; }
    int GetViewId() const { return viewId; }

    Math::Vector2<double> GetSize() const;

    bool IsDirty() const { return dirtyFlag; }
    void MarkDirty() { dirtyFlag = true; }
    void ClearDirty() { dirtyFlag = false; }
protected:
    PlatformWindow* attachedWindow = nullptr;

    int viewId;
    static int nextViewId;

    bool dirtyFlag = true;
    bool updateEveryFrame = true;

    std::vector<IRenderable*> renderables;

    MulticastEvent<>::Listener* windowResizedListener = nullptr;
};

REFLECTION_END()