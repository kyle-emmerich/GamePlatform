#pragma once
#include <cmath>

template<typename T>
class Vector2 {
public:
	union {
		struct { T X, Y; };
		T XY[2];
	};

	Vector2() : X(0), Y(0) {}
	Vector2(T x, T y) : X(x), Y(y) {}

	Vector2<T> operator+(const Vector2<T>& other) const {
		return Vector2<T>(X + other.X, Y + other.Y);
	}
	Vector2<T> operator-(const Vector2<T>& other) const {
		return Vector2<T>(X - other.X, Y - other.Y);
	}
	Vector2<T> operator*(T scalar) const {
		return Vector2<T>(X * scalar, Y * scalar);
	}
	Vector2<T> operator/(T scalar) const {
		return Vector2<T>(X / scalar, Y / scalar);
	}
	bool operator==(const Vector2<T>& other) const {
		return X == other.X && Y == other.Y;
	}
	bool operator!=(const Vector2<T>& other) const {
		return !(*this == other);
	}
	T Dot(const Vector2<T>& other) const {
		return X * other.X + Y * other.Y;
	}
	Vector2<T> Perpendicular() const {
		return Vector2<T>(-Y, X);
	}
	Vector2<T> Cross(Vector2<T> other) const {
		return Vector2<T>(X * other.Y - Y * other.X, Y * other.X - X * other.Y);
	}
	Vector2<T> Reflect(const Vector2<T>& normal) const {
		T dotProduct = this->Dot(normal);
		return *this - normal * (2 * dotProduct);
	}
	Vector2<T> ProjectOnto(const Vector2<T>& other) const {
		T dotProduct = this->Dot(other);
		T otherMagnitudeSq = other.Dot(other);
		if (otherMagnitudeSq == 0) return Vector2<T>(0, 0);
		return other * (dotProduct / otherMagnitudeSq);
	}
	Vector2<T> Lerp(const Vector2<T>& other, T t) const {
		return Vector2<T>(X + (other.X - X) * t, Y + (other.Y - Y) * t);
	}
	Vector2<T> Intersection(const Vector2<T>& other, T& outDistance) {
		T denom = X * other.Y - Y * other.X;
		if (denom == 0) {
			outDistance = 0;
			return Vector2<T>(0, 0); // Parallel lines
		}
		T t = (other.X * Y - other.Y * X) / denom;
		outDistance = t;
		return Vector2<T>(X * t, Y * t);
	}

	T Magnitude() const {
		return std::sqrt(X * X + Y * Y);
	}
	Vector2<T> Unit() const {
		T mag = Magnitude();
		if (mag == 0) return Vector2<T>(0, 0);
		return Vector2<T>(X / mag, Y / mag);
	}
	// inline static const Vector2<T> Zero = Vector2<T>(0, 0);
	// inline static const Vector2<T> One = Vector2<T>(1, 1);
	// inline static const Vector2<T> UnitX = Vector2<T>(1, 0);
	// inline static const Vector2<T> UnitY = Vector2<T>(0, 1);
};