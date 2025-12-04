#pragma once

#include "Math/Vector3.h"

template<typename T>
class Ray {
public:
	Vector3<T> Origin;
	Vector3<T> Direction;

	Ray() : Origin(Vector3<T>::Zero), Direction(Vector3<T>::UnitZ) {}
	Ray(const Vector3<T>& origin, const Vector3<T>& direction)
		: Origin(origin), Direction(direction.Normalize()) {}

	Vector3<T> GetPoint(T distance) const {
		return Origin + Direction.Unit() * distance;
	}
};