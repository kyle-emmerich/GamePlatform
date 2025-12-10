#pragma once

#include "Instance/ObjectInstance.h"
#include "Attachment.generated.h"

class [[reflect()]] Attachment : public ObjectInstance, BaseInstance<Attachment> {
    REFLECTION()
public:
    Attachment(Engine* engine) : ObjectInstance(engine) {}

    [[reflect(Derived)]]
    [[summary("The primary axis of the attachment. This corresponds to the right vector of the attachment's transform.")]]
    Math::Vector3<double> GetPrimaryAxis() const;

    [[reflect(Derived)]]
    [[summary("The secondary axis of the attachment. This corresponds to the up vector of the attachment's transform.")]]
    Math::Vector3<double> GetSecondaryAxis() const;

    [[reflect(Derived)]]
    [[summary("The world transform of the attachment.")]]
    Math::Transform<double> GetWorldTransform() const;
protected:


};

REFLECTION_END()