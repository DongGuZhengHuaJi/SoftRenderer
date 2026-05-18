#pragma once
#include <cmath>

struct Vec2 {
    Vec2() : x(0), y(0) {}
    Vec2(float _x, float _y) : x(_x), y(_y) {}

    Vec2 operator+(const Vec2& o) const { return Vec2(x + o.x, y + o.y); }
    Vec2 operator-(const Vec2& o) const { return Vec2(x - o.x, y - o.y); }
    Vec2 operator*(float s) const { return Vec2(x * s, y * s); }
    Vec2 operator/(float s) const { return Vec2(x / s, y / s); }
    Vec2& operator+=(const Vec2& o) { x += o.x; y += o.y; return *this; }
    Vec2& operator-=(const Vec2& o) { x -= o.x; y -= o.y; return *this; }

    float dot(const Vec2& o) const { return x * o.x + y * o.y; }
    float cross(const Vec2& o) const { return x * o.y - y * o.x; }
    float length() const { return std::sqrt(x * x + y * y); }

    float x;
    float y;
};

struct Rect {
    float xMin, yMin, xMax, yMax;

    Rect() : xMin(0), yMin(0), xMax(0), yMax(0) {}
    Rect(float x0, float y0, float x1, float y1)
        : xMin(std::min(x0, x1)), yMin(std::min(y0, y1)),
          xMax(std::max(x0, x1)), yMax(std::max(y0, y1)) {}

    bool contains(const Vec2& p) const {
        return p.x >= xMin && p.x <= xMax && p.y >= yMin && p.y <= yMax;
    }
};

// 3x3 matrix for 2D affine transforms (column-major storage).
// Indices:
//   | m[0] m[3] m[6] |   | m00 m01 m02 |
//   | m[1] m[4] m[7] | = | m10 m11 m12 |
//   | m[2] m[5] m[8] |   |  0   0   1  |
struct Mat3 {
    float m[9];

    Mat3() {
        m[0]=1; m[1]=0; m[2]=0;
        m[3]=0; m[4]=1; m[5]=0;
        m[6]=0; m[7]=0; m[8]=1;
    }

    static Mat3 identity() { return Mat3(); }

    Mat3 inverse() const {
        Mat3 r;
        float a = m[0], c = m[3], tx = m[6];
        float b = m[1], d = m[4], ty = m[7];

        // 计算左上角 2x2 矩阵的行列式
        float det = a * d - b * c;
        if (std::abs(det) < 1e-6f) return r; // 防止除以 0 导致崩溃

        float invDet = 1.0f / det;
        
        // 填充逆矩阵的缩放、旋转、错切分量
        r.m[0] = d * invDet;
        r.m[3] = -c * invDet;
        r.m[1] = -b * invDet;
        r.m[4] = a * invDet;

        // 填充逆矩阵的平移分量
        r.m[6] = -(d * tx - c * ty) * invDet;
        r.m[7] = -(-b * tx + a * ty) * invDet;

        // 最后一行固定为 0, 0, 1
        r.m[2] = 0; r.m[5] = 0; r.m[8] = 1;

        return r;
    }

    static Mat3 translate(float tx, float ty) {
        Mat3 r;
        r.m[6] = tx;
        r.m[7] = ty;
        return r;
    }

    static Mat3 scale(float sx, float sy) {
        Mat3 r;
        r.m[0] = sx;
        r.m[4] = sy;
        return r;
    }

    static Mat3 rotate(float radians) {
        Mat3 r;
        float c = std::cos(radians);
        float s = std::sin(radians);
        r.m[0] = c;  r.m[3] = -s;
        r.m[1] = s;  r.m[4] =  c;
        return r;
    }

    static Mat3 reflectX() {
        Mat3 r;
        r.m[0] =  1;
        r.m[4] = -1;
        return r;
    }

    static Mat3 reflectY() {
        Mat3 r;
        r.m[0] = -1;
        r.m[4] =  1;
        return r;
    }

    static Mat3 shear(float shx, float shy) {
        Mat3 r;
        r.m[3] = shx;  // m01
        r.m[1] = shy;  // m10
        return r;
    }

    Mat3 operator*(const Mat3& o) const {
        Mat3 r;
        for (int col = 0; col < 3; col++) {
            int ci = col * 3;
            for (int row = 0; row < 3; row++) {
                r.m[ci + row] =
                    m[0 + row] * o.m[ci + 0] +
                    m[3 + row] * o.m[ci + 1] +
                    m[6 + row] * o.m[ci + 2];
            }
        }
        return r;
    }

    Vec2 transform(const Vec2& v) const {
        float x = m[0] * v.x + m[3] * v.y + m[6];
        float y = m[1] * v.x + m[4] * v.y + m[7];
        return Vec2(x, y);
    }
};
