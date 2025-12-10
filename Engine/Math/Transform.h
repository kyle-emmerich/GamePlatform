#pragma once

#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Quaternion.h"
//#include "Scripting/LuaDataType.h"

namespace Math {

template<typename T = double>
class Transform {
public:
	union {
		struct {
			T m00, m01, m02, m03;
			T m10, m11, m12, m13;
			T m20, m21, m22, m23;
			T m30, m31, m32, m33;
		};
		struct {
			Vector4<T> Right;
			Vector4<T> Up;
			Vector4<T> Backward;
			Vector4<T> Translation;
		};
		T m[4][4];
		T mm[16];
	};
public:
	Transform() {
		// Identity matrix
		m00 = 1.0; m01 = 0.0; m02 = 0.0; m03 = 0.0;
		m10 = 0.0; m11 = 1.0; m12 = 0.0; m13 = 0.0;
		m20 = 0.0; m21 = 0.0; m22 = 1.0; m23 = 0.0;
		m30 = 0.0; m31 = 0.0; m32 = 0.0; m33 = 1.0;
	}

	Transform(const Transform& other) {
		for (int i = 0; i < 16; ++i) mm[i] = other.mm[i];
	}

	Transform& operator=(const Transform& other) {
		if (this != &other) {
			for (int i = 0; i < 16; ++i) mm[i] = other.mm[i];
		}
		return *this;
	}

	void SetTranslation(const Vector3<T>& translation) {
		m03 = translation.X;
		m13 = translation.Y;
		m23 = translation.Z;
	}
	Vector3<T> GetTranslation() const {
		return Vector3<T>(m03, m13, m23);
	}

	void SetRotation(const Quaternion<T>& rotation) {
		T xx = rotation.X * rotation.X;
		T yy = rotation.Y * rotation.Y;
		T zz = rotation.Z * rotation.Z;
		T xy = rotation.X * rotation.Y;
		T xz = rotation.X * rotation.Z;
		T yz = rotation.Y * rotation.Z;
		T wx = rotation.W * rotation.X;
		T wy = rotation.W * rotation.Y;
		T wz = rotation.W * rotation.Z;
		m00 = 1.0 - 2.0 * (yy + zz);
		m01 = 2.0 * (xy - wz);
		m02 = 2.0 * (xz + wy);
		m10 = 2.0 * (xy + wz);
		m11 = 1.0 - 2.0 * (xx + zz);
		m12 = 2.0 * (yz - wx);
		m20 = 2.0 * (xz - wy);
		m21 = 2.0 * (yz + wx);
		m22 = 1.0 - 2.0 * (xx + yy);
	}
	Quaternion<T> GetRotation() const {
		T trace = m00 + m11 + m22;
		Quaternion<T> q;
		if (trace > 0) {
			T s = sqrt(trace + 1.0) * 2.0;
			q.W = 0.25 * s;
			q.X = (m21 - m12) / s;
			q.Y = (m02 - m20) / s;
			q.Z = (m10 - m01) / s;
		} else if ((m00 > m11) && (m00 > m22)) {
			T s = sqrt(1.0 + m00 - m11 - m22) * 2.0;
			q.W = (m21 - m12) / s;
			q.X = 0.25 * s;
			q.Y = (m01 + m10) / s;
			q.Z = (m02 + m20) / s;
		} else if (m11 > m22) {
			T s = sqrt(1.0 + m11 - m00 - m22) * 2.0;
			q.W = (m02 - m20) / s;
			q.X = (m01 + m10) / s;
			q.Y = 0.25 * s;
			q.Z = (m12 + m21) / s;
		} else {
			T s = sqrt(1.0 + m22 - m00 - m11) * 2.0;
			q.W = (m10 - m01) / s;
			q.X = (m02 + m20) / s;
			q.Y = (m12 + m21) / s;
			q.Z = 0.25 * s;
		}
		return q.Normalize();
	}

	void SetScale(const Vector3<T>& scale) {
		m00 = scale.X;
		m11 = scale.Y;
		m22 = scale.Z;
	}
	Vector3<T> GetScale() const {
		return Vector3<T>(
			sqrt(m00 * m00 + m01 * m01 + m02 * m02),
			sqrt(m10 * m10 + m11 * m11 + m12 * m12),
			sqrt(m20 * m20 + m21 * m21 + m22 * m22)
		);
	}

	constexpr Transform<T> operator*(const Transform<T>& other) const {
		Transform<T> result;
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				result.m[i][j] = m[i][0] * other.m[0][j] +
								 m[i][1] * other.m[1][j] +
								 m[i][2] * other.m[2][j] +
								 m[i][3] * other.m[3][j];
			}
		}
		return result;
	}

	Vector3<T> TransformPoint(const Vector3<T>& point) const {
		return Vector3<T>(
			m00 * point.X + m01 * point.Y + m02 * point.Z + m03,
			m10 * point.X + m11 * point.Y + m12 * point.Z + m13,
			m20 * point.X + m21 * point.Y + m22 * point.Z + m23
		);
	}
	Vector3<T> TransformVector(const Vector3<T>& vec) const {
		return Vector3<T>(
			m00 * vec.X + m01 * vec.Y + m02 * vec.Z,
			m10 * vec.X + m11 * vec.Y + m12 * vec.Z,
			m20 * vec.X + m21 * vec.Y + m22 * vec.Z
		);
	}

	Transform<T> Inverse() const {
        // Ensure the matrix is affine (bottom row should be [0, 0, 0, 1])
        if (m30 != 0 || m31 != 0 || m32 != 0 || m33 != 1) {
			throw std::runtime_error("Matrix is not affine and cannot be inverted using this method.");
        }

        Transform<T> inv;

        // Invert the 3x3 rotation/scale matrix
        T det = m00 * (m11 * m22 - m12 * m21) -
        m01 * (m10 * m22 - m12 * m20) +
        m02 * (m10 * m21 - m11 * m20);
        if (det == 0) {
			throw std::runtime_error("Cannot invert a singular matrix");
        }

        T invDet = 1.0 / det;
        inv.m00 = (m11 * m22 - m12 * m21) * invDet;
        inv.m01 = (m02 * m21 - m01 * m22) * invDet;
        inv.m02 = (m01 * m12 - m02 * m11) * invDet;
        inv.m10 = (m12 * m20 - m10 * m22) * invDet;
        inv.m11 = (m00 * m22 - m02 * m20) * invDet;
        inv.m12 = (m02 * m10 - m00 * m12) * invDet;
        inv.m20 = (m10 * m21 - m11 * m20) * invDet;
        inv.m21 = (m01 * m20 - m00 * m21) * invDet;
        inv.m22 = (m00 * m11 - m01 * m10) * invDet;

        // Compute the inverse translation
        inv.m03 = -(inv.m00 * m03 + inv.m01 * m13 + inv.m02 * m23);
        inv.m13 = -(inv.m10 * m03 + inv.m11 * m13 + inv.m12 * m23);
        inv.m23 = -(inv.m20 * m03 + inv.m21 * m13 + inv.m22 * m23);

        // Set the bottom row to [0, 0, 0, 1]
        inv.m30 = 0.0;
        inv.m31 = 0.0;
        inv.m32 = 0.0;
        inv.m33 = 1.0;

        return inv;
	}

	Transform<T> Lerp(Transform<T> Other, T Alpha) const {
		Transform<T> Result;
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				Result.m[i][j] = m[i][j] + (Other.m[i][j] - m[i][j]) * Alpha;
			}
		}
		return Result;
	}

	Transform<T> operator+ (const Vector3<T>& translation) const {
		Transform<T> result = *this;
		result.m03 += translation.X;
		result.m13 += translation.Y;
		result.m23 += translation.Z;
		return result;
	}
	Transform<T> operator- (const Vector3<T>& translation) const {
		Transform<T> result = *this;
		result.m03 -= translation.X;
		result.m13 -= translation.Y;
		result.m23 -= translation.Z;
		return result;
	}
	Transform<T> operator* (T scalar) const {
		Transform<T> result;
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				result.m[i][j] = m[i][j] * scalar;
			}
		}
		return result;
	}
};

}

/*
namespace Lua {
	template<>
	void registerType<Transform<NumberType>>(State* state) {
		static bool bInit = false;
		static Lua::DataType TransformType;

		if (!bInit) {
			bInit = true;
			TransformType.name = "Transform";
			TransformType.__new = [](Lua::State& _state) -> int {
				int args = _state.top();
				if (args == 0) {
					auto* transform = _state.newUserData<Transform<Lua::NumberType>>();
					return 1;
				}
				return _state.error("Invalid arguments to Transform constructor");
			};
			TransformType.__index = [](Lua::State& _state) -> int {
				int args = _state.top();
				if (args < 2) {
					return _state.error("Not enough arguments to Transform __index");
				}
				Transform<Lua::NumberType>* transform = *(Transform<Lua::NumberType>**)(luaL_checkudata(_state, 1, "Transform"));
				std::string key = luaL_checkstring(_state, 2);

			};

			state->registerType(TransformType);
		}
		
	}
}*/