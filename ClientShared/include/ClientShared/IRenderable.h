#pragma once

class Viewport;

class IRenderable
{
public:
    virtual ~IRenderable() = default;

    virtual void OnAttached(Viewport* viewport) = 0;
    virtual void OnDetached(Viewport* viewport) = 0;
    virtual void OnRendered(Viewport* viewport) = 0;
};
