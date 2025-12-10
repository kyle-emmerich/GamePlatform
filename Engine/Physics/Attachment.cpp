#include "Physics/Attachment.h"
#include "Math/Transform.h"

Math::Vector3<double> Attachment::GetPrimaryAxis() const {
    return Math::Vector3<double>(WorldTransform.Right.X, WorldTransform.Right.Y, WorldTransform.Right.Z);
}

Math::Vector3<double> Attachment::GetSecondaryAxis() const {
    return Math::Vector3<double>(WorldTransform.Up.X, WorldTransform.Up.Y, WorldTransform.Up.Z);
}

Math::Transform<double> Attachment::GetWorldTransform() const {
    return WorldTransform;
}
