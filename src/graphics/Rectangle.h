#include "Shape.h"
#include "math/Vec2.h"
#include <cstdint>

class Rectangle : public Shape {
public:
    Rectangle(const Vec2& topLeft, const Vec2& bottomRight, uint32_t color)
        : Shape(5), topLeft(topLeft), bottomRight(bottomRight), color(color) {}
    ~Rectangle() = default;
    Vec2 topLeft;
    Vec2 bottomRight;
    uint32_t color;
};