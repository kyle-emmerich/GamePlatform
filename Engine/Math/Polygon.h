#pragma once

#include <vector>
#include "Math/Vector2.h"
#include "Math/Rect.h"

namespace Math {

template<typename T>
class Polygon {
public:
	std::vector<Vector2<T>> Vertices;

	Polygon() = default;
	Polygon(std::initializer_list<Vector2<T>> vertices)
		: Vertices(vertices) {}

	T Area() const {
		T area = 0;
		size_t n = Vertices.size();
		for (size_t i = 0; i < n; i++) {
			const Vector2<T>& v1 = Vertices[i];
			const Vector2<T>& v2 = Vertices[(i + 1) % n];
			area += (v1.x * v2.y) - (v2.x * v1.y);
		}
		return area * 0.5;
	}
	T Perimeter() const {
		T perimeter = 0;
		size_t n = Vertices.size();
		for (size_t i = 0; i < n; i++) {
			const Vector2<T>& v1 = Vertices[i];
			const Vector2<T>& v2 = Vertices[(i + 1) % n];
			perimeter += (v2 - v1).Length();
		}
		return perimeter;
	}

	bool IsConvex() const {
		size_t n = Vertices.size();
		if (n < 4) {
			return true; // Triangles are always convex
		}
		bool sign = false;
		for (size_t i = 0; i < n; i++) {
			const Vector2<T>& v1 = Vertices[i];
			const Vector2<T>& v2 = Vertices[(i + 1) % n];
			const Vector2<T>& v3 = Vertices[(i + 2) % n];
			T crossProduct = (v2.x - v1.x) * (v3.y - v2.y) - (v2.y - v1.y) * (v3.x - v2.x);
			if (i == 0) {
				sign = crossProduct > 0;
			} else if ((crossProduct > 0) != sign) {
				return false;
			}
		}
		return true;
	}

	bool ContainsPoint(const Vector2<T>& point) const {
		size_t n = Vertices.size();
		bool inside = false;
		for (size_t i = 0, j = n - 1; i < n; j = i++) {
			const Vector2<T>& vi = Vertices[i];
			const Vector2<T>& vj = Vertices[j];
			if (((vi.y > point.y) != (vj.y > point.y)) &&
				(point.x < (vj.x - vi.x) * (point.y - vi.y) / (vj.y - vi.y) + vi.x)) {
				inside = !inside;
			}
		}
		return inside;
	}

	void EnsureClockwise() {
		if (Area() < 0) {
			std::reverse(Vertices.begin(), Vertices.end());
		}
	}

	bool Intersects(const Polygon<T>& other) const {
		// Check if any vertex of this polygon is inside the other polygon
		for (const auto& vertex : Vertices) {
			if (other.ContainsPoint(vertex)) {
				return true;
			}
		}
		// Check if any vertex of the other polygon is inside this polygon
		for (const auto& vertex : other.Vertices) {
			if (ContainsPoint(vertex)) {
				return true;
			}
		}
		return false;
	}

	Rect<T> BoundingBox() const {
		if (Vertices.empty()) {
			return Rect<T>(0, 0, 0, 0);
		}
		T minX = Vertices[0].x;
		T maxX = Vertices[0].x;
		T minY = Vertices[0].y;
		T maxY = Vertices[0].y;
		for (const auto& vertex : Vertices) {
			if (vertex.x < minX) minX = vertex.x;
			if (vertex.x > maxX) maxX = vertex.x;
			if (vertex.y < minY) minY = vertex.y;
			if (vertex.y > maxY) maxY = vertex.y;
		}
		return Rect<T>(minX, minY, maxX - minX, maxY - minY);
	}

	Vector2<T> Centroid() const {
		T cx = 0, cy = 0;
		T area = Area();
		size_t n = Vertices.size();

		for (size_t i = 0; i < n; i++) {
			const Vector2<T>& v1 = Vertices[i];
			const Vector2<T>& v2 = Vertices[(i + 1) % n];
			T factor = (v1.x * v2.y - v2.x * v1.y);
			cx += (v1.x + v2.x) * factor;
			cy += (v1.y + v2.y) * factor;
		}

		cx /= (6 * area);
		cy /= (6 * area);
		return Vector2<T>(cx, cy);
	}

	void Translate(const Vector2<T>& offset) {
		for (auto& vertex : Vertices) {
			vertex += offset;
		}
	}

	void Rotate(T angle, const Vector2<T>& origin = Vector2<T>(0, 0)) {
		T cosTheta = std::cos(angle);
		T sinTheta = std::sin(angle);
		for (auto& vertex : Vertices) {
			T x = vertex.x - origin.x;
			T y = vertex.y - origin.y;
			vertex.x = origin.x + x * cosTheta - y * sinTheta;
			vertex.y = origin.y + x * sinTheta + y * cosTheta;
		}
	}
	void RotateAroundCentroid(T angle) {
		Rotate(angle, Centroid());
	}

	static Polygon<T> FromRect(const Rect<T>& rect) {
		return Polygon<T>({
			Vector2<T>(rect.left, rect.top),
			Vector2<T>(rect.right, rect.top),
			Vector2<T>(rect.right, rect.bottom),
			Vector2<T>(rect.left, rect.bottom)
			});
	}

	static Polygon<T> RegularPolygon(size_t sides, T radius, const Vector2<T>& center = Vector2<T>(0, 0)) {
		Polygon<T> poly;
		if (sides < 3) return poly; // Need at least 3 sides for a polygon
		T angleStep = (2 * M_PI) / sides;
		for (size_t i = 0; i < sides; i++) {
			T angle = i * angleStep;
			poly.Vertices.push_back(Vector2<T>(
				center.x + radius * std::cos(angle),
				center.y + radius * std::sin(angle)
			));
		}
		return poly;
	}
	static Polygon<T> Star(size_t points, T innerRadius, T outerRadius, const Vector2<T>& center = Vector2<T>(0, 0)) {
		Polygon<T> poly;
		if (points < 2) return poly; // Need at least 2 points for a star
		T angleStep = M_PI / points;
		for (size_t i = 0; i < points * 2; i++) {
			T radius = (i % 2 == 0) ? outerRadius : innerRadius;
			T angle = i * angleStep;
			poly.Vertices.push_back(Vector2<T>(
				center.x + radius * std::cos(angle),
				center.y + radius * std::sin(angle)
			));
		}
		return poly;
	}
};

}