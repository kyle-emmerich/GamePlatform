#pragma once

#include "Math/Vector3.h"

template<typename T>
class AABB {
public:
	Vector3<T> Min;
	Vector3<T> Max;

	AABB() : Min(Vector3<T>::Zero), Max(Vector3<T>::Zero) {}
	AABB(const Vector3<T>& min, const Vector3<T>& max) : Min(min), Max(max) {}

	bool Contains(const Vector3<T>& point) const {
		return (point.X >= Min.X && point.X <= Max.X) &&
			   (point.Y >= Min.Y && point.Y <= Max.Y) &&
			   (point.Z >= Min.Z && point.Z <= Max.Z);
	}
	bool Contains(const AABB<T>& other) const {
		return (other.Min.X >= Min.X && other.Max.X <= Max.X) &&
			   (other.Min.Y >= Min.Y && other.Max.Y <= Max.Y) &&
			   (other.Min.Z >= Min.Z && other.Max.Z <= Max.Z);
	}
	bool IsContainedBy(const AABB<T>& other) const {
		return other.Contains(*this);
	}
	bool Intersects(const AABB<T>& other) const {
		return (Min.X <= other.Max.X && Max.X >= other.Min.X) &&
			   (Min.Y <= other.Max.Y && Max.Y >= other.Min.Y) &&
			   (Min.Z <= other.Max.Z && Max.Z >= other.Min.Z);
	}
	
	AABB<T> Union(const AABB<T>& other) const {
		return AABB<T>(
			Vector3<T>(std::min(Min.X, other.Min.X), std::min(Min.Y, other.Min.Y), std::min(Min.Z, other.Min.Z)),
			Vector3<T>(std::max(Max.X, other.Max.X), std::max(Max.Y, other.Max.Y), std::max(Max.Z, other.Max.Z))
		);
	}
	AABB<T> Intersection(const AABB<T>& other) const {
		if (!Intersects(other)) {
			return AABB<T>(Vector3<T>::Zero, Vector3<T>::Zero); // No intersection
		}
		return AABB<T>(
			Vector3<T>(std::max(Min.X, other.Min.X), std::max(Min.Y, other.Min.Y), std::max(Min.Z, other.Min.Z)),
			Vector3<T>(std::min(Max.X, other.Max.X), std::min(Max.Y, other.Max.Y), std::min(Max.Z, other.Max.Z))
		);
	}
	T Volume() const {
		return (Max.X - Min.X) * (Max.Y - Min.Y) * (Max.Z - Min.Z);
	}
	Vector3<T> Center() const {
		return Vector3<T>((Min.X + Max.X) / 2, (Min.Y + Max.Y) / 2, (Min.Z + Max.Z) / 2);
	}
	Vector3<T> Size() const {
		return Vector3<T>(Max.X - Min.X, Max.Y - Min.Y, Max.Z - Min.Z);
	}
	void Expand(const Vector3<T>& amount) {
		Min = Min - amount;
		Max = Max + amount;
	}
	void Translate(const Vector3<T>& offset) {
		Min = Min + offset;
		Max = Max + offset;
	}
	void Scale(T factor) {
		Vector3<T> center = Center();
		Vector3<T> halfSize = Size() * (factor / 2);
		Min = center - halfSize;
		Max = center + halfSize;
	}

	void ExpandToFit(const Vector3<T>& point) {
		Min.X = std::min(Min.X, point.X);
		Min.Y = std::min(Min.Y, point.Y);
		Min.Z = std::min(Min.Z, point.Z);
		Max.X = std::max(Max.X, point.X);
		Max.Y = std::max(Max.Y, point.Y);
		Max.Z = std::max(Max.Z, point.Z);
	}

	static AABB<T> FromPoints(const std::vector<Vector3<T>>& points) {
		if (points.empty()) {
			return AABB<T>(Vector3<T>::Zero, Vector3<T>::Zero);
		}
		Vector3<T> min = points[0];
		Vector3<T> max = points[0];
		for (const auto& point : points) {
			min.X = std::min(min.X, point.X);
			min.Y = std::min(min.Y, point.Y);
			min.Z = std::min(min.Z, point.Z);
			max.X = std::max(max.X, point.X);
			max.Y = std::max(max.Y, point.Y);
			max.Z = std::max(max.Z, point.Z);
		}
		return AABB<T>(min, max);
	}

	static AABB<T> Infinite() {
		T inf = std::numeric_limits<T>::infinity();
		return AABB<T>(Vector3<T>(inf, inf, inf), Vector3<T>(-inf, -inf, -inf));
	}
	static AABB<T> ZeroBox() {
		return AABB<T>(Vector3<T>::Zero, Vector3<T>::Zero);
	}
	static AABB<T> UnitBox() {
		return AABB<T>(Vector3<T>(0, 0, 0), Vector3<T>(1, 1, 1));
	}
	static AABB<T> FromCenterSize(const Vector3<T>& center, const Vector3<T>& size) {
		Vector3<T> halfSize = size * 0.5;
		return AABB<T>(center - halfSize, center + halfSize);
	}

	AABB<T> Quantize(T gridSize) const {
		return AABB<T>(
			Vector3<T>(
				std::floor(Min.X / gridSize) * gridSize,
				std::floor(Min.Y / gridSize) * gridSize,
				std::floor(Min.Z / gridSize) * gridSize
			),
			Vector3<T>(
				std::ceil(Max.X / gridSize) * gridSize,
				std::ceil(Max.Y / gridSize) * gridSize,
				std::ceil(Max.Z / gridSize) * gridSize
			)
		);
	}
};