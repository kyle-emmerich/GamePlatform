#pragma once

#include "Math/Vector3.h"
#include "Math/Ray.h"

namespace Math {

template<typename T>
class Plane {
public:
	Vector3<T> Point;
	Vector3<T> Normal;

	Plane() : Point(0, 0, 0), Normal(0, 1, 0) {}
	Plane(const Vector3<T>& point, const Vector3<T>& normal) : Point(point), Normal(normal.Normalize()) {}
	Plane(const Vector3<T>& a, const Vector3<T>& b, const Vector3<T>& c) {
		Vector3<T> ab = b - a;
		Vector3<T> ac = c - a;
		Normal = ab.Cross(ac).Normalize();
		Point = a;
	}
	T DistanceToPoint(const Vector3<T>& p) const {
		return Normal.Dot(p - Point);
	}
	bool IntersectRay(const Ray<T>& ray, T& outT) const {
		T denom = Normal.Dot(ray.Direction);
		if (std::abs(denom) > 1e-6) { // Not parallel
			Vector3<T> p0l0 = Point - ray.Origin;
			outT = p0l0.Dot(Normal) / denom;
			return (outT >= 0);
		}
		return false; // Parallel
	}

	bool Intersection(const Plane<T>& other, Ray<T>& outRay) const {
		Vector3<T> direction = Normal.Cross(other.Normal);
		T denom = direction.LengthSquared();
		if (denom < 1e-6) {
			// Planes are parallel
			return false;
		}
		// Find a point on the intersection line
		T d1 = -Normal.Dot(Point);
		T d2 = -other.Normal.Dot(other.Point);
		Vector3<T> point = (other.Normal * d1 - Normal * d2).Cross(direction) / denom;
		outRay = Ray<T>(point, direction.Normalize());
		return true;
	}

	Plane<T> Flipped() const {
		return Plane<T>(Point, -Normal);
	}

	static const Plane<T> XY() { return Plane<T>(Vector3<T>(0, 0, 0), Vector3<T>(0, 0, 1)); }
	static const Plane<T> YZ() { return Plane<T>(Vector3<T>(0, 0, 0), Vector3<T>(1, 0, 0)); }
	static const Plane<T> ZX() { return Plane<T>(Vector3<T>(0, 0, 0), Vector3<T>(0, 1, 0)); }
	
	static const Plane<T> XNegativeY() { return Plane<T>(Vector3<T>(0, 0, 0), Vector3<T>(0, 0, -1)); }
	static const Plane<T> YNegativeZ() { return Plane<T>(Vector3<T>(0, 0, 0), Vector3<T>(-1, 0, 0)); }
	static const Plane<T> ZNegativeX() { return Plane<T>(Vector3<T>(0, 0, 0), Vector3<T>(0, -1, 0)); }

};

}