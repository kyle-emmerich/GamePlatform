#include "UI/UIScreenLayer.h"
#include "Platform/Viewport.h"

Math::Transform<double> UIScreenLayer::GetLayerTransform() {
    // if (!Viewport) {
    //     return Math::Transform<double>();
    // }
    // //For screen layers, we can just return an orthographic projection transform.
    // //Get the viewport information to calculate the projection.
    // Math::Vector2<double> viewportSize = Viewport->GetSize();
    // return Math::Transform<double>::OrthographicProjection(
    //     0.0, viewportSize.X,
    //     viewportSize.Y, 0.0,
    //     -1.0, 1.0
    // );
    return Math::Transform<double>();
}