#pragma once

#include "Math/Vector3.h"
#include "Math/AABB.h"

template<typename T>
class Sphere {
public:
	Vector3<T> Center;
	T Radius;

	Sphere() : Center(0, 0, 0), Radius(1) {}
	Sphere(const Vector3<T>& center, T radius) : Center(center), Radius(radius) {}

	bool Contains(const Vector3<T>& point) const {
		Vector3<T> diff = point - Center;
		return diff.LengthSquared() <= Radius * Radius;
	}
	bool Contains(const Sphere<T>& other) const {
		Vector3<T> diff = other.Center - Center;
		T radiusDiff = Radius - other.Radius;
		return radiusDiff >= 0 && diff.LengthSquared() <= radiusDiff * radiusDiff;
	}
	bool Contains(const AABB<T>& box) const {
		// Check all 8 corners of the AABB
		for (int x = 0; x <= 1; ++x) {
			for (int y = 0; y <= 1; ++y) {
				for (int z = 0; z <= 1; ++z) {
					Vector3<T> corner(
						x ? box.Max.X : box.Min.X,
						y ? box.Max.Y : box.Min.Y,
						z ? box.Max.Z : box.Min.Z
					);
					if (!Contains(corner)) {
						return false;
					}
				}
			}
		}
		return true;
	}
	bool IsContainedBy(const Sphere<T>& other) const {
		return other.Contains(*this);
	}
	bool IsContainedBy(const AABB<T>& box) const {
		// Check if the sphere's center is within the AABB shrunken by the sphere's radius
		AABB<T> shrunkenBox(
			Vector3<T>(box.Min.X + Radius, box.Min.Y + Radius, box.Min.Z + Radius),
			Vector3<T>(box.Max.X - Radius, box.Max.Y - Radius, box.Max.Z - Radius)
		);
		return shrunkenBox.Contains(Center);
	}
	bool Intersects(const Sphere<T>& other) const {
		Vector3<T> diff = other.Center - Center;
		T radiusSum = Radius + other.Radius;
		return diff.LengthSquared() <= radiusSum * radiusSum;
	}

	T Volume() const {
		return (4.0 / 3.0) * 3.14159265358979323846 * Radius * Radius * Radius;
	}
	T SurfaceArea() const {
		return 4.0 * 3.14159265358979323846 * Radius * Radius;
	}
	static Sphere<T> Lerp(const Sphere<T>& a, const Sphere<T>& b, T t) {
		return Sphere<T>(a.Center.Lerp(b.Center, t), a.Radius + (b.Radius - a.Radius) * t);
	}
	
	static Sphere<T> Union(const Sphere<T>& a, const Sphere<T>& b) {
		Vector3<T> centerDiff = b.Center - a.Center;
		T distSq = centerDiff.LengthSquared();
		T radiusDiff = b.Radius - a.Radius;
		// If one sphere is completely inside the other, return the larger sphere
		if (radiusDiff * radiusDiff >= distSq) {
			return (a.Radius > b.Radius) ? a : b;
		}
		T dist = std::sqrt(distSq);
		T newRadius = (dist + a.Radius + b.Radius) * 0.5;
		Vector3<T> newCenter;
		if (dist > 0) {
			newCenter = a.Center + centerDiff * ((newRadius - a.Radius) / dist);
		} else {
			newCenter = a.Center; // Spheres are at the same position
		}
		return Sphere<T>(newCenter, newRadius);
	}

	void ExpandToFit(const Vector3<T>& point) {
		Vector3<T> diff = point - Center;
		T distSq = diff.LengthSquared();
		if (distSq > Radius * Radius) {
			T dist = std::sqrt(distSq);
			T newRadius = (Radius + dist) * 0.5;
			Center += diff * ((newRadius - Radius) / dist);
			Radius = newRadius;
		}
	}

	static Sphere<T> FromPoints(const std::initializer_list<Vector3<T>>& points) {
		if (points.empty()) {
			return Sphere<T>(Vector3<T>(0, 0, 0), 1);
		}
		Vector3<T> center = points[0];
		T radius = 0;
		for (const auto& point : points) {
			Vector3<T> diff = point - center;
			T distSq = diff.LengthSquared();
			if (distSq > radius * radius) {
				T dist = std::sqrt(distSq);
				T newRadius = (radius + dist) * 0.5;
				if (dist > 0) {
					center += diff * ((newRadius - radius) / dist);
				}
				radius = newRadius;
			}
		}
		return Sphere<T>(center, radius);
	}
	static Sphere<T> FromPoints(const std::vector<Vector3<T>>& points) {
		return FromPoints(std::initializer_list<Vector3<T>>(&points.at(0), &points.at(points.size())));
	}

	void ExpandToTouch(const Sphere<T>& other) {
		Vector3<T> diff = other.Center - Center;
		T distSq = diff.LengthSquared();
		T radiusSum = Radius + other.Radius;
		if (distSq > radiusSum * radiusSum) {
			T dist = std::sqrt(distSq);
			T newRadius = (Radius + dist + other.Radius) * 0.5;
			if (dist > 0) {
				Center += diff * ((newRadius - Radius) / dist);
			}
			Radius = newRadius;
		}
	}

	void Translate(const Vector3<T>& translation) {
		Center += translation;
	}
	void Scale(T scale) {
		if (scale > 0) {
			Radius *= scale;
		}
	}

	Plane<T> TangentPlane(const Vector3<T> Direction) {
		Vector3<T> unitDir = Direction.Unit();
		return Plane<T>(unitDir, -(Center.Dot(unitDir) + Radius));
	}
	bool IntersectionPlane(const Sphere<T>& other, Plane<T>& outPlane) {
		Vector3<T> centerDiff = other.Center - Center;
		T distSq = centerDiff.LengthSquared();
		T radiusSum = Radius + other.Radius;
		if (distSq > radiusSum * radiusSum) {
			return false; // No intersection
		}
		T dist = std::sqrt(distSq);
		if (dist == 0) {
			// Spheres are at the same position, return arbitrary plane
			outPlane = Plane<T>(Vector3<T>(1, 0, 0), -Radius);
			return true;
		}
		T d = (Radius * Radius - other.Radius * other.Radius + distSq) / (2 * dist);
		Vector3<T> intersectionCenter = Center + centerDiff * (d / dist);
		Vector3<T> normal = centerDiff.Unit();
		outPlane = Plane<T>(normal, -intersectionCenter.Dot(normal));
		return true;
	}
};