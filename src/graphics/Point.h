#pragma once
#include "math/Vec2.h"
#include <cstdint>
#include "Shape.h"
class Point : public Shape {
public:
    Point(const Vec2& position, uint32_t color)
        : Shape(1), position(position), color(color) {}
    ~Point() = default;
    Vec2 position;
    uint32_t color;
};