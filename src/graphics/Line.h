#pragma once
#include "math/Vec2.h"
#include <cstdint>
#include "Shape.h"

class Line : public Shape {
public:
    Line(const Vec2& start, const Vec2& end, uint32_t color)
        : Shape(2), start(start), end(end), color(color) {}
    ~Line() = default;
    Vec2 start;
    Vec2 end;
    uint32_t color;
};