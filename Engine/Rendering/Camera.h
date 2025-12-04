#pragma once

#include "Instance/ObjectInstance.h"
#include "Camera.generated.h"

class [[reflect()]] Camera : public ObjectInstance, BaseInstance<Camera> {
    REFLECTION()
public:
    Camera(Engine* engine) : ObjectInstance(engine) {}

    [[reflect()]]
    [[summary("The field of view of the camera in degrees.")]]
    double FieldOfView = 90.0;

    [[reflect()]]
    [[summary("The near clipping plane distance.")]]
    double NearPlane = 0.1;

    [[reflect()]]
    [[summary("The far clipping plane distance.")]]
    double FarPlane = 10000.0;

    
};

REFLECTION_END()