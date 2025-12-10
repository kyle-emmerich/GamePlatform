#pragma once

#include "Math/Vector2.h"

namespace Math {
	
template<typename T = double>
class Vector3 {
public:
	union {
		struct {
			T X;
			T Y;
			T Z;
		};
		struct {
			Vector2<T> XY;
			T _;
		};
		T XYZ[3];
	};

	Vector3() : X(0), Y(0), Z(0) {}
	Vector3(T x, T y, T z) : X(x), Y(y), Z(z) {}
	Vector3(Vector2<T> xy, T z = 0) : X(xy.X), Y(xy.Y), Z(z) {}

	Vector3(const Vector3<T>& other) : X(other.X), Y(other.Y), Z(other.Z) {}
	Vector3<T>& operator=(const Vector3<T>& other) {
		if (this != &other) {
			X = other.X;
			Y = other.Y;
			Z = other.Z;
		}
		return *this;
	}
	bool operator==(const Vector3<T>& other) const {
		return (X == other.X && Y == other.Y && Z == other.Z);
	}
	bool operator!=(const Vector3<T>& other) const {
		return !(*this == other);
	}
	T& operator[](int index) {
		return XYZ[index];
	}
	const T& operator[](int index) const {
		return XYZ[index];
	}
	T Length() const {
		return static_cast<T>(sqrt(X * X + Y * Y + Z * Z));
	}
	T LengthSquared() const {
		return (X * X + Y * Y + Z * Z);
	}
	Vector3<T> Unit() const {
		T len = Length();
		if (len == 0) return Vector3<T>(0, 0, 0);
		return Vector3<T>(X / len, Y / len, Z / len);
	}

	Vector3<T> operator+(const Vector3<T>& other) const {
		return Vector3<T>(X + other.X, Y + other.Y, Z + other.Z);
	}
	Vector3<T> operator-(const Vector3<T>& other) const {
		return Vector3<T>(X - other.X, Y - other.Y, Z - other.Z);
	}
	Vector3<T> operator*(T scalar) const {
		return Vector3<T>(X * scalar, Y * scalar, Z * scalar);
	}
	Vector3<T> operator/(T scalar) const {
		if (scalar == 0) throw std::runtime_error("Division by zero");
		return Vector3<T>(X / scalar, Y / scalar, Z / scalar);
	}
	Vector3<T> Cross(const Vector3<T>& other) const {
		return Vector3<T>(
			Y * other.Z - Z * other.Y,
			Z * other.X - X * other.Z,
			X * other.Y - Y * other.X
		);
	}
	Vector3<T> operator-() const {
		return Vector3<T>(-X, -Y, -Z);
	}
	template<typename U>
	Vector3<U> Cast() const {
		return Vector3<U>(static_cast<U>(X), static_cast<U>(Y), static_cast<U>(Z));
	}
	template<typename U>
	Vector3<T> Lerp(const Vector3<T>& other, U t) const {
		return Vector3<T>(
			X + (other.X - X) * t,
			Y + (other.Y - Y) * t,
			Z + (other.Z - Z) * t
		);
	}

	T Dot(const Vector3<T>& other) const {
		return X * other.X + Y * other.Y + Z * other.Z;
	}

	Vector3<T> Reflect(const Vector3<T>& normal) const {
		// R = V - 2 * (V . N) * N
		T dotProduct = this->Dot(normal);
		return *this - normal * (2 * dotProduct);
	}
	Vector3<T> ProjectOnto(const Vector3<T>& other) const {
		// Projection of A onto B = (A . B / B . B) * B
		T otherLengthSquared = other.LengthSquared();
		if (otherLengthSquared == 0) return Vector3<T>(0, 0, 0);
		T dotProduct = this->Dot(other);
		return other * (dotProduct / otherLengthSquared);
	}

	bool Parallel(Vector3<T> other, T tolerance = static_cast<T>(1e-6)) const {
		Vector3<T> crossProduct = this->Cross(other);
		return crossProduct.LengthSquared() <= tolerance * tolerance;
	}
	bool Orthogonal(Vector3<T> other, T tolerance = static_cast<T>(1e-6)) const {
		T dotProduct = this->Dot(other);
		return std::abs(dotProduct) <= tolerance;
	}

	static const Vector3<T> Zero;
	static const Vector3<T> One;
	static const Vector3<T> UnitX;
	static const Vector3<T> UnitY;
	static const Vector3<T> UnitZ;
};

template<typename T>
const Vector3<T> Vector3<T>::Zero = Vector3<T>(0, 0, 0);
template<typename T>
const Vector3<T> Vector3<T>::One = Vector3<T>(1, 1, 1);
template<typename T>
const Vector3<T> Vector3<T>::UnitX = Vector3<T>(1, 0, 0);
template<typename T>
const Vector3<T> Vector3<T>::UnitY = Vector3<T>(0, 1, 0);
template<typename T>
const Vector3<T> Vector3<T>::UnitZ = Vector3<T>(0, 0, 1);

}