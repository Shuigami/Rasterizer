#pragma once

#include "vector.h"
#include <array>
#include <cmath>

class Matrix4x4 {
public:
    std::array<float, 16> m;

    Matrix4x4() {
        m = {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };
    }

    float& operator()(int row, int col) {
        return m[row * 4 + col];
    }

    float operator()(int row, int col) const {
        return m[row * 4 + col];
    }

    static Matrix4x4 identity() {
        return Matrix4x4();
    }

    static Matrix4x4 translation(float x, float y, float z) {
        Matrix4x4 result;
        result(0, 3) = x;
        result(1, 3) = y;
        result(2, 3) = z;
        return result;
    }

    static Matrix4x4 scaling(float x, float y, float z) {
        Matrix4x4 result;
        result(0, 0) = x;
        result(1, 1) = y;
        result(2, 2) = z;
        return result;
    }

    static Matrix4x4 rotationX(float angleRadians) {
        Matrix4x4 result;
        float c = std::cos(angleRadians);
        float s = std::sin(angleRadians);

        result(1, 1) = c;
        result(1, 2) = -s;
        result(2, 1) = s;
        result(2, 2) = c;

        return result;
    }

    static Matrix4x4 rotationY(float angleRadians) {
        Matrix4x4 result;
        float c = std::cos(angleRadians);
        float s = std::sin(angleRadians);

        result(0, 0) = c;
        result(0, 2) = s;
        result(2, 0) = -s;
        result(2, 2) = c;

        return result;
    }

    static Matrix4x4 rotationZ(float angleRadians) {
        Matrix4x4 result;
        float c = std::cos(angleRadians);
        float s = std::sin(angleRadians);

        result(0, 0) = c;
        result(0, 1) = -s;
        result(1, 0) = s;
        result(1, 1) = c;

        return result;
    }

    static Matrix4x4 perspective(float fovY, float aspect, float zNear, float zFar) {
        Matrix4x4 result;

        float tanHalfFovY = std::tan(fovY / 2.0f);

        result(0, 0) = 1.0f / (aspect * tanHalfFovY);
        result(1, 1) = 1.0f / tanHalfFovY;
        result(2, 2) = -(zFar + zNear) / (zFar - zNear);
        result(2, 3) = -(2.0f * zFar * zNear) / (zFar - zNear);
        result(3, 2) = -1.0f;
        result(3, 3) = 0.0f;

        return result;
    }

    static Matrix4x4 lookAt(const Vec3& eye, const Vec3& target, const Vec3& up) {
        Vec3 f = (target - eye).normalized();
        Vec3 s = f.cross(up).normalized();
        Vec3 u = s.cross(f);

        Matrix4x4 result;
        result(0, 0) = s.x;
        result(0, 1) = s.y;
        result(0, 2) = s.z;
        result(1, 0) = u.x;
        result(1, 1) = u.y;
        result(1, 2) = u.z;
        result(2, 0) = -f.x;
        result(2, 1) = -f.y;
        result(2, 2) = -f.z;
        result(0, 3) = -s.dot(eye);
        result(1, 3) = -u.dot(eye);
        result(2, 3) = f.dot(eye);

        return result;
    }

    Matrix4x4 operator*(const Matrix4x4& other) const {
        Matrix4x4 result;

        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                result(i, j) = 0.0f;
                for (int k = 0; k < 4; k++) {
                    result(i, j) += (*this)(i, k) * other(k, j);
                }
            }
        }

        return result;
    }

    Vec4 operator*(const Vec4& v) const {
        return Vec4(
            m[0] * v.x + m[1] * v.y + m[2] * v.z + m[3] * v.w,
            m[4] * v.x + m[5] * v.y + m[6] * v.z + m[7] * v.w,
            m[8] * v.x + m[9] * v.y + m[10] * v.z + m[11] * v.w,
            m[12] * v.x + m[13] * v.y + m[14] * v.z + m[15] * v.w
        );
    }
};
