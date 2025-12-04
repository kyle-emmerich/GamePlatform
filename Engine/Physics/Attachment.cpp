#include "Physics/Attachment.h"
#include "Math/Transform.h"

Vector3<double> Attachment::GetPrimaryAxis() const {
    return Vector3<double>(WorldTransform.Right.X, WorldTransform.Right.Y, WorldTransform.Right.Z);
}

Vector3<double> Attachment::GetSecondaryAxis() const {
    return Vector3<double>(WorldTransform.Up.X, WorldTransform.Up.Y, WorldTransform.Up.Z);
}

Transform<double> Attachment::GetWorldTransform() const {
    return WorldTransform;
}
