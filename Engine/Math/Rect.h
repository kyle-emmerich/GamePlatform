#pragma once

#include "Math/Vector2.h"
#include <vector>
#include <algorithm>

namespace Math {

template<typename T>
class Rect {
public:
	union {
		struct {
			T left;
			T top;
			T right;
			T bottom;
		};
		struct {
			Vector2<T> min;
			Vector2<T> max;
		};
	};

	Rect() : left(0), top(0), right(0), bottom(0) {}
	Rect(T l, T t, T r, T b) : left(l), top(t), right(r), bottom(b) {}
	Rect(const Vector2<T>& min, const Vector2<T>& max) : min(min), max(max) {}

	bool Contains(const Vector2<T>& point) const {
		return point.X >= left && point.X <= right && point.Y >= top && point.Y <= bottom;
	}
	bool Intersects(const Rect<T>& other) const {
		return !(other.left > right || other.right < left || other.top > bottom || other.bottom < top);
	}
	T Area() const {
		return (right - left) * (bottom - top);
	}
	Vector2<T> Size() const {
		return Vector2<T>(right - left, bottom - top);
	}
	Vector2<T> Center() const {
		return Vector2<T>((left + right) / 2, (top + bottom) / 2);
	}
	static Rect<T> FromCenterSize(const Vector2<T>& center, const Vector2<T>& size) {
		return Rect<T>(center.X - size.X / 2, center.Y - size.Y / 2, center.X + size.X / 2, center.Y + size.Y / 2);
	}
	static Rect<T> Containing(const Rect<T>& a, const Rect<T>& b) {
		return Rect<T>(std::min(a.left, b.left), std::min(a.top, b.top), std::max(a.right, b.right), std::max(a.bottom, b.bottom));
	}
	static Rect<T> Containing(std::vector<Vector2<T>> points) {
		if (points.empty()) return Rect<T>(0, 0, 0, 0);
		T minX = points[0].X;
		T minY = points[0].Y;
		T maxX = points[0].X;
		T maxY = points[0].Y;
		for (const Vector2<T>& p : points) {
			if (p.X < minX) minX = p.X;
			if (p.Y < minY) minY = p.Y;
			if (p.X > maxX) maxX = p.X;
			if (p.Y > maxY) maxY = p.Y;
		}
		return Rect<T>(minX, minY, maxX, maxY);
	}

	// inline static const Rect<T> Zero = Rect<T>(0, 0, 0, 0);
	// inline static const Rect<T> Unit = Rect<T>(0, 0, 1, 1);
};

}