#pragma once

#include "Math/Vector3.h"
#include "Math/Transform.h"

namespace Math {

template<typename T>
class Cuboid {
public:
	Transform<T> Transform;
	Vector3<T> Size;

	Cuboid() : Size(1, 1, 1) {}
	Cuboid(const Transform<T>& transform, const Vector3<T>& size) : Transform(transform), Size(size) {}

	Vector3<T> GetMin() const {
		Vector3<T> halfSize = Size * 0.5;
		return Transform.TransformPoint(Vector3<T>(-halfSize.X, -halfSize.Y, -halfSize.Z));
	}
	Vector3<T> GetMax() const {
		Vector3<T> halfSize = Size * 0.5;
		return Transform.TransformPoint(Vector3<T>(halfSize.X, halfSize.Y, halfSize.Z));
	}
	bool ContainsPoint(const Vector3<T>& point) const {
		Vector3<T> localPoint = Transform.Inverse().TransformPoint(point);
		Vector3<T> halfSize = Size * 0.5;
		return (localPoint.X >= -halfSize.X && localPoint.X <= halfSize.X) &&
			   (localPoint.Y >= -halfSize.Y && localPoint.Y <= halfSize.Y) &&
			   (localPoint.Z >= -halfSize.Z && localPoint.Z <= halfSize.Z);
	}
	


};

}