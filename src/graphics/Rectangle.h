#pragma once
#include "Shape.h"
#include "math/Vec2.h"
#include <cstdint>

class Rectangle : public Shape {
public:
    Rectangle(const Vec2& topLeft, const Vec2& bottomRight, uint32_t color)
        : color(color) {
        v[0] = topLeft;
        v[1] = Vec2(bottomRight.x, topLeft.y);
        v[2] = bottomRight;
        v[3] = Vec2(topLeft.x, bottomRight.y);
    }
    ~Rectangle() = default;

    void draw(Renderer& renderer) const override;
    void transform(const Mat3& matrix) override;

    Vec2 v[4];   // 4 corner vertices in order
    uint32_t color;
};
