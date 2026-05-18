#pragma once
#include "math/Vec2.h"
#include <cstdint>
#include "Shape.h"
#include "math/Vec2.h"

class Circle : public Shape {
public:
    Circle(const Vec2& center, float radius, uint32_t color)
        : center(center), radius(radius), color(color), localToWorld(Mat3::identity()) {
            localToWorld = Mat3::translate(center.x, center.y) * Mat3::scale(radius, radius) * localToWorld;
        }
    ~Circle() = default;

    void draw(Renderer& renderer) const override;
    void transform(const Mat3& matrix) override;

    Vec2 center;
    float radius;
    uint32_t color;

    Mat3 localToWorld = Mat3::identity(); // For future transformations
};
