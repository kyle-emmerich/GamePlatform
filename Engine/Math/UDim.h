#pragma once

template<typename T>
struct UDim {
    T Scale = 0;
    T Offset = 0;

    UDim() = default;
    UDim(T scale, T offset) : Scale(scale), Offset(offset) {}

    T Resolve(T parentSize) const {
        return Scale * parentSize + Offset;
    }

    UDim<T> operator+(const UDim<T>& other) const {
        return UDim<T>(Scale + other.Scale, Offset + other.Offset);
    }
    UDim<T> operator-(const UDim<T>& other) const {
        return UDim<T>(Scale - other.Scale, Offset - other.Offset);
    }

    UDim<T> operator*(T scalar) const {
        return UDim<T>(Scale * scalar, Offset * scalar);
    }
    UDim<T> operator/(T scalar) const {
        return UDim<T>(Scale / scalar, Offset / scalar);
    }
    UDim<T> operator+=(const UDim<T>& other) {
        Scale += other.Scale;
        Offset += other.Offset;
        return *this;
    }
    UDim<T> operator-=(const UDim<T>& other) {
        Scale -= other.Scale;
        Offset -= other.Offset;
        return *this;
    }
    UDim<T> operator*=(T scalar) {
        Scale *= scalar;
        Offset *= scalar;
        return *this;
    }
    UDim<T> operator/=(T scalar) {
        Scale /= scalar;
        Offset /= scalar;
        return *this;
    }
    bool operator==(const UDim<T>& other) const {
        return Scale == other.Scale && Offset == other.Offset;
    }
    bool operator!=(const UDim<T>& other) const {
        return !(*this == other);
    }
};