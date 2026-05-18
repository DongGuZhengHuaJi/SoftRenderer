#pragma once
#include "math/Vec2.h"
#include <cstdint>
#include "Shape.h"

class Point : public Shape {
public:
    Point(const Vec2& position, uint32_t color)
        : position(position), color(color) {}
    ~Point() = default;

    void draw(Renderer& renderer) const override;
    void transform(const Mat3& matrix) override;

    Vec2 position;
    uint32_t color;
};
