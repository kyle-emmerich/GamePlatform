#pragma once

#include "Math/UDim.h"

namespace Math {
	
template<typename T>
struct UDim2 {
    UDim<T> X;
    UDim<T> Y;

    UDim2() = default;
    UDim2(UDim<T> x, UDim<T> y) : X(x), Y(y) {}
    UDim2(T xScale, T xOffset, T yScale, T yOffset) : X(xScale, xOffset), Y(yScale, yOffset) {}

    Vector2<T> Resolve(const Vector2<T>& parentSize) const {
        return Vector2<T>(X.Resolve(parentSize.X), Y.Resolve(parentSize.Y));
    }

    UDim2<T> operator+(const UDim2<T>& other) const {
        return UDim2<T>(X + other.X, Y + other.Y);
    }
    UDim2<T> operator-(const UDim2<T>& other) const {
        return UDim2<T>(X - other.X, Y - other.Y);
    }
    UDim2<T> operator*(T scalar) const {
        return UDim2<T>(X * scalar, Y * scalar);
    }
    UDim2<T> operator/(T scalar) const {
        return UDim2<T>(X / scalar, Y / scalar);
    }
    UDim2<T> operator+=(const UDim2<T>& other) {
        X += other.X;
        Y += other.Y;
        return *this;
    }
    UDim2<T> operator-=(const UDim2<T>& other) {
        X -= other.X;
        Y -= other.Y;
        return *this;
    }
    UDim2<T> operator*=(T scalar) {
        X *= scalar;
        Y *= scalar;
        return *this;
    }
    UDim2<T> operator/=(T scalar) {
        X /= scalar;
        Y /= scalar;
        return *this;
    }
    bool operator==(const UDim2<T>& other) const {
        return X == other.X && Y == other.Y;
    }
    bool operator!=(const UDim2<T>& other) const {
        return !(*this == other);
    }

};

}