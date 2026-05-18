#pragma once
#include "math/Vec2.h"
#include <cstdint>
#include "Shape.h"

class Line : public Shape {
public:
    Line(const Vec2& start, const Vec2& end, uint32_t color)
        : start(start), end(end), color(color) {}
    ~Line() = default;

    void draw(Renderer& renderer) const override;
    void transform(const Mat3& matrix) override;

    Vec2 start;
    Vec2 end;
    uint32_t color;
};
