#pragma once

#include "Math/Vector3.h"

namespace Math {

template<typename T = double>
class Quaternion {
public:
	union {
		struct {
			T X;
			T Y;
			T Z;
			T W;
		};
		T XYZW[4];
	};
	
	Quaternion() : X(0), Y(0), Z(0), W(1) {}
	Quaternion(T x, T y, T z, T w) : X(x), Y(y), Z(z), W(w) {}
	Quaternion(const Quaternion<T>& other) : X(other.X), Y(other.Y), Z(other.Z), W(other.W) {}
	Quaternion<T>& operator=(const Quaternion<T>& other) {
		if (this != &other) {
			X = other.X;
			Y = other.Y;
			Z = other.Z;
			W = other.W;
		}
		return *this;
	}
	bool operator==(const Quaternion<T>& other) const {
		return (X == other.X && Y == other.Y && Z == other.Z && W == other.W);
	}
	bool operator!=(const Quaternion<T>& other) const {
		return !(*this == other);
	}
	T& operator[](int index) {
		return XYZW[index];
	}
	const T& operator[](int index) const {
		return XYZW[index];
	}
	Quaternion<T> Conjugate() const {
		return Quaternion<T>(-X, -Y, -Z, W);
	}
	Quaternion<T> Inverse() const {
		T normSq = X * X + Y * Y + Z * Z + W * W;
		if (normSq == 0) throw std::runtime_error("Cannot invert a zero-length quaternion");
		return Conjugate() / normSq;
	}
	Quaternion<T> Normalize() const {
		T len = Length();
		if (len == 0) return Quaternion<T>(0, 0, 0, 1);
		return Quaternion<T>(X / len, Y / len, Z / len, W / len);
	}
	T Length() const {
		return static_cast<T>(sqrt(X * X + Y * Y + Z * Z + W * W));
	}
	T LengthSquared() const {
		return (X * X + Y * Y + Z * Z + W * W);
	}
	Quaternion<T> operator*(const Quaternion<T>& other) const {
		return Quaternion<T>(
			W * other.X + X * other.W + Y * other.Z - Z * other.Y,
			W * other.Y - X * other.Z + Y * other.W + Z * other.X,
			W * other.Z + X * other.Y - Y * other.X + Z * other.W,
			W * other.W - X * other.X - Y * other.Y - Z * other.Z
		);
	}
	Quaternion<T> operator*(T scalar) const {
		return Quaternion<T>(X * scalar, Y * scalar, Z * scalar, W * scalar);
	}
	Quaternion<T> operator/(T scalar) const {
		if (scalar == 0) throw std::runtime_error("Division by zero");
		return Quaternion<T>(X / scalar, Y / scalar, Z / scalar, W / scalar);
	}
	Vector3<T> Rotate(const Vector3<T>& v) const {
		Quaternion<T> qv(v.X, v.Y, v.Z, 0);
		Quaternion<T> result = (*this) * qv * this->Inverse();
		return Vector3<T>(result.X, result.Y, result.Z);
	}
	template<typename U>
	Quaternion<U> Cast() const {
		return Quaternion<U>(static_cast<U>(X), static_cast<U>(Y), static_cast<U>(Z), static_cast<U>(W));
	}

	Quaternion<T> SLerp(const Quaternion<T>& other, T t) const {
		// Compute the cosine of the angle between the two quaternions
		T cosHalfTheta = W * other.W + X * other.X + Y * other.Y + Z * other.Z;
		// If cosHalfTheta < 0, the interpolation will take the long way around the sphere.
		// To fix this, one quaternion must be negated.
		if (cosHalfTheta < 0) {
			cosHalfTheta = -cosHalfTheta;
		}
		// If the quaternions are very close, use linear interpolation
		if (fabs(cosHalfTheta) >= 1.0) {
			return Quaternion<T>(X, Y, Z, W);
		}
		// Calculate temporary values
		T halfTheta = acos(cosHalfTheta);
		T sinHalfTheta = sqrt(1.0 - cosHalfTheta * cosHalfTheta);
		// If sinHalfTheta is close to zero, use linear interpolation
		if (fabs(sinHalfTheta) < 0.001) {
			return Quaternion<T>(
				X * 0.5 + other.X * 0.5,
				Y * 0.5 + other.Y * 0.5,
				Z * 0.5 + other.Z * 0.5,
				W * 0.5 + other.W * 0.5
			).Normalize();
		}
		T ratioA = sin((1 - t) * halfTheta) / sinHalfTheta;
		T ratioB = sin(t * halfTheta) / sinHalfTheta;
		return Quaternion<T>(
			X * ratioA + other.X * ratioB,
			Y * ratioA + other.Y * ratioB,
			Z * ratioA + other.Z * ratioB,
			W * ratioA + other.W * ratioB
		).Normalize();
	}

	Quaternion<T> Lerp(const Quaternion<T>& other, T t) const {
		return Quaternion<T>(
			X + (other.X - X) * t,
			Y + (other.Y - Y) * t,
			Z + (other.Z - Z) * t,
			W + (other.W - W) * t
		).Normalize();
	}
	static Quaternion<T> FromAxisAngle(const Vector3<T>& axis, T angle) {
		T halfAngle = angle * static_cast<T>(0.5);
		T s = static_cast<T>(sin(halfAngle));
		return Quaternion<T>(
			axis.X * s,
			axis.Y * s,
			axis.Z * s,
			static_cast<T>(cos(halfAngle))
		).Normalize();
	}

	static Quaternion<T> FromEulerAngles(T pitch, T yaw, T roll) {
		T cy = cos(yaw * 0.5);
		T sy = sin(yaw * 0.5);
		T cp = cos(pitch * 0.5);
		T sp = sin(pitch * 0.5);
		T cr = cos(roll * 0.5);
		T sr = sin(roll * 0.5);
		return Quaternion<T>(
			sr * cp * cy - cr * sp * sy,
			cr * sp * cy + sr * cp * sy,
			cr * cp * sy - sr * sp * cy,
			cr * cp * cy + sr * sp * sy
		).Normalize();
	}

	static Quaternion<T> Identity() {
		return Quaternion<T>(0, 0, 0, 1);
	}
};

}