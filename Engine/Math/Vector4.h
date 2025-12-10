#pragma once

#include "Math/Vector2.h"
#include "Math/Vector3.h"

namespace Math {

template<typename T>
class Vector4 {
public:
	union {
		struct {
			T X, Y, Z, W;
		};
		struct {
			Vector3<T> XYZ;
			T __paddingW;
		};
		struct {
			T __paddingX;
			Vector3<T> YZW;
		};
		struct {
			Vector2<T> XY;
			Vector2<T> ZW;
		};
		T XYZW[4];
	};

	Vector4() : X(0), Y(0), Z(0), W(0) {}
	Vector4(T x, T y, T z, T w) : X(x), Y(y), Z(z), W(w) {}
	Vector4(const Vector3<T>& vec3, T w) : X(vec3.X), Y(vec3.Y), Z(vec3.Z), W(w) {}
	Vector4(const Vector2<T>& vec2a, const Vector2<T>& vec2b) : X(vec2a.X), Y(vec2a.Y), Z(vec2b.X), W(vec2b.Y) {}
	Vector4(const Vector4<T>& other) : X(other.X), Y(other.Y), Z(other.Z), W(other.W) {}
	Vector4<T>& operator=(const Vector4<T>& other) {
		if (this != &other) {
			X = other.X;
			Y = other.Y;
			Z = other.Z;
			W = other.W;
		}
		return *this;
	}
	T& operator[](size_t index) {
		if (index >= 4) {
			throw std::out_of_range("Vector4 index out of range");
		}
		return XYZW[index];
	}
	const T& operator[](size_t index) const {
		if (index >= 4) {
			throw std::out_of_range("Vector4 index out of range");
		}
		return XYZW[index];
	}
	Vector4<T> operator+(const Vector4<T>& other) const {
		return Vector4<T>(X + other.X, Y + other.Y, Z + other.Z, W + other.W);
	}
	Vector4<T> operator-(const Vector4<T>& other) const {
		return Vector4<T>(X - other.X, Y - other.Y, Z - other.Z, W - other.W);
	}
	Vector4<T> operator*(T scalar) const {
		return Vector4<T>(X * scalar, Y * scalar, Z * scalar, W * scalar);
	}
	Vector4<T> operator/(T scalar) const {
		if (scalar == 0) {
			throw std::invalid_argument("Division by zero in Vector4");
		}
		return Vector4<T>(X / scalar, Y / scalar, Z / scalar, W / scalar);
	}
	bool operator==(const Vector4<T>& other) const {
		return X == other.X && Y == other.Y && Z == other.Z && W == other.W;
	}
	bool operator!=(const Vector4<T>& other) const {
		return !(*this == other);
	}
	T Length() const {
		return std::sqrt(X * X + Y * Y + Z * Z + W * W);
	}
	T LengthSquared() const {
		return X * X + Y * Y + Z * Z + W * W;
	}
	Vector4<T> Unit() const {
		T len = Length();
		if (len == 0) {
			throw std::invalid_argument("Cannot normalize zero-length vector");
		}
		return *this / len;
	}
	T Dot(const Vector4<T>& b) {
		return X * b.X + Y * b.Y + Z * b.Z + W * b.W;
	}
	Vector4<T> Lerp(const Vector4<T>& b, T t) {
		return (*this) + (b - (*this)) * t;
	}

	operator Vector3<T>() const {
		return Vector3<T>(X, Y, Z);
	}
	operator Vector2<T>() const {
		return Vector2<T>(X, Y);
	}

	static const Vector4<T> Zero;
	static const Vector4<T> One;
	static const Vector4<T> UnitX;
	static const Vector4<T> UnitY;
	static const Vector4<T> UnitZ;
	static const Vector4<T> UnitW;

};

template<typename T>
const Vector4<T> Vector4<T>::Zero = Vector4<T>(0, 0, 0, 0);
template<typename T>
const Vector4<T> Vector4<T>::One = Vector4<T>(1, 1, 1, 1);
template<typename T>
const Vector4<T> Vector4<T>::UnitX = Vector4<T>(1, 0, 0, 0);
template<typename T>
const Vector4<T> Vector4<T>::UnitY = Vector4<T>(0, 1, 0, 0);
template<typename T>
const Vector4<T> Vector4<T>::UnitZ = Vector4<T>(0, 0, 1, 0);
template<typename T>
const Vector4<T> Vector4<T>::UnitW = Vector4<T>(0, 0, 0, 1);

}