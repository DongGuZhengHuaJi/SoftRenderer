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
