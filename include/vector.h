#pragma once

#include <cmath>
#include <algorithm>
#include <cstdint>

struct Color {
    uint8_t r, g, b, a;

    Color() : r(0), g(0), b(0), a(255) {}
    Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255)
        : r(red), g(green), b(blue), a(alpha) {}

    uint32_t toUint32() const {
        return (a << 24) | (b << 16) | (g << 8) | r;
    }

    static Color fromUint32(uint32_t color) {
        return Color(
            color & 0xFF,
            (color >> 8) & 0xFF,
            (color >> 16) & 0xFF,
            (color >> 24) & 0xFF
        );
    }

    Color operator*(float scalar) const {
        return Color(
            static_cast<uint8_t>(std::clamp(r * scalar, 0.0f, 255.0f)),
            static_cast<uint8_t>(std::clamp(g * scalar, 0.0f, 255.0f)),
            static_cast<uint8_t>(std::clamp(b * scalar, 0.0f, 255.0f)),
            a
        );
    }

    Color operator+(const Color& other) const {
        return Color(
            static_cast<uint8_t>(std::clamp(r + other.r, 0, 255)),
            static_cast<uint8_t>(std::clamp(g + other.g, 0, 255)),
            static_cast<uint8_t>(std::clamp(b + other.b, 0, 255)),
            static_cast<uint8_t>(std::clamp(a + other.a, 0, 255))
        );
    }
};

class Vec2 {
public:
    float x, y;

    Vec2() : x(0.0f), y(0.0f) {}
    Vec2(float x, float y) : x(x), y(y) {}

    Vec2 operator+(const Vec2& v) const { return Vec2(x + v.x, y + v.y); }
    Vec2 operator-(const Vec2& v) const { return Vec2(x - v.x, y - v.y); }
    Vec2 operator*(float scalar) const { return Vec2(x * scalar, y * scalar); }
    Vec2 operator/(float scalar) const { return Vec2(x / scalar, y / scalar); }

    float length() const { return std::sqrt(x * x + y * y); }
    Vec2 normalized() const {
        float len = length();
        if (len < 1e-6f) return *this;
        return *this / len;
    }

    float dot(const Vec2& v) const { return x * v.x + y * v.y; }
};

class Vec3 {
public:
    float x, y, z;

    Vec3() : x(0.0f), y(0.0f), z(0.0f) {}
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

    Vec3 operator+(const Vec3& v) const { return Vec3(x + v.x, y + v.y, z + v.z); }
    Vec3 operator-(const Vec3& v) const { return Vec3(x - v.x, y - v.y, z - v.z); }
    Vec3 operator*(float scalar) const { return Vec3(x * scalar, y * scalar, z * scalar); }
    Vec3 operator/(float scalar) const { return Vec3(x / scalar, y / scalar, z / scalar); }
    Vec3 operator-() const { return Vec3(-x, -y, -z); }

    float length() const { return std::sqrt(x * x + y * y + z * z); }
    Vec3 normalized() const {
        float len = length();
        if (len < 1e-6f) return *this;
        return *this / len;
    }

    float dot(const Vec3& v) const { return x * v.x + y * v.y + z * v.z; }
    Vec3 cross(const Vec3& v) const {
        return Vec3(
            y * v.z - z * v.y,
            z * v.x - x * v.z,
            x * v.y - y * v.x
        );
    }
};

class Vec4 {
public:
    float x, y, z, w;

    Vec4() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}
    Vec4(float x, float y, float z, float w = 1.0f) : x(x), y(y), z(z), w(w) {}
    Vec4(const Vec3& v, float w = 1.0f) : x(v.x), y(v.y), z(v.z), w(w) {}

    Vec4 operator+(const Vec4& v) const { return Vec4(x + v.x, y + v.y, z + v.z, w + v.w); }
    Vec4 operator-(const Vec4& v) const { return Vec4(x - v.x, y - v.y, z - v.z, w - v.w); }
    Vec4 operator*(float scalar) const { return Vec4(x * scalar, y * scalar, z * scalar, w * scalar); }
    Vec4 operator/(float scalar) const { return Vec4(x / scalar, y / scalar, z / scalar, w / scalar); }
    Vec4 operator-() const { return Vec4(-x, -y, -z, -w); }

    Vec3 toVec3() const {
        if (std::abs(w) < 1e-6f) return Vec3(x, y, z);
        return Vec3(x / w, y / w, z / w);
    }

    float dot(const Vec4& v) const { return x * v.x + y * v.y + z * v.z + w * v.w; }
};
