#pragma once

#include <cinttypes>
#include "Math/Vector4.h"
#include <cmath>

namespace Math {

struct Color {
     float R, G, B, A;

    Color() : R(1.0f), G(1.0f), B(1.0f), A(1.0f) {}
    Color(float _r, float _g, float _b, float _a = 1.0f) : R(_r), G(_g), B(_b), A(_a) {}
    Color(uint32_t hexValue) : Color(FromHex(hexValue)) {}
    Color(const Vector4<float>& vec) : R(vec.X), G(vec.Y), B(vec.Z), A(vec.W) {}

    static Color White() { return Color(1.0f, 1.0f, 1.0f); }
    static Color Black() { return Color(0.0f, 0.0f, 0.0f); }
    static Color Red() { return Color(1.0f, 0.0f, 0.0f); }
    static Color Green() { return Color(0.0f, 1.0f, 0.0f); }
    static Color Blue() { return Color(0.0f, 0.0f, 1.0f); }
    static Color Transparent() { return Color(0.0f, 0.0f, 0.0f, 0.0f); }

    static Color FromHex(uint32_t hexValue) {
        float r = ((hexValue >> 16) & 0xFF) / 255.0f;
        float g = ((hexValue >> 8) & 0xFF) / 255.0f;
        float b = (hexValue & 0xFF) / 255.0f;
        return Color(r, g, b, 1.0f);
    }
    uint32_t ToHex() const {
        uint32_t rInt = static_cast<uint32_t>(R * 255.0f) & 0xFF;
        uint32_t gInt = static_cast<uint32_t>(G * 255.0f) & 0xFF;
        uint32_t bInt = static_cast<uint32_t>(B * 255.0f) & 0xFF;
        return (rInt << 16) | (gInt << 8) | bInt;
    }

    uint32_t ToABGR() const {
        uint32_t r = static_cast<uint32_t>(R * 255.0f) & 0xFF;
        uint32_t g = static_cast<uint32_t>(G * 255.0f) & 0xFF;
        uint32_t b = static_cast<uint32_t>(B * 255.0f) & 0xFF;
        uint32_t a = static_cast<uint32_t>(A * 255.0f) & 0xFF;
        return (a << 24) | (b << 16) | (g << 8) | r;
    }

    static Color FromHSV(float hue, float saturation, float value, float alpha = 1.0f) {
        float h = hue / 360.0f;
        float s = saturation;
        float v = value;
        float c = v * s;
        float x = c * (1.0f - std::abs(std::fmod(h * 6.0f, 2.0f) - 1.0f));
        float m = v - c;
        float r, g, b;
        if (h * 6.0f >= 0 && h * 6.0f < 1) {
            r = c;
            g = x;
            b = 0;
        } else if (h * 6.0f >= 1 && h * 6.0f < 2) {
            r = x;
            g = c;
            b = 0;
        } else if (h * 6.0f >= 2 && h * 6.0f < 3) {
            r = 0;
            g = c;
            b = x;
        } else if (h * 6.0f >= 3 && h * 6.0f < 4) {
            r = 0;
            g = x;
            b = c;
        } else if (h * 6.0f >= 4 && h * 6.0f < 5) {
            r = x;
            g = 0;
            b = c;
        } else {
            r = c;
            g = 0;
            b = x;
        }
        return Color(r + m, g + m, b + m, alpha);
    }
    static Vector4<float> ToVector4(const Color& color) {
        return Vector4<float>(color.R, color.G, color.B, color.A);
    }

    static Color Lerp(const Color& a, const Color& b, float t) {
        return Color(
            a.R + (b.R - a.R) * t,
            a.G + (b.G - a.G) * t,
            a.B + (b.B - a.B) * t,
            a.A + (b.A - a.A) * t
        );
    }

};

}