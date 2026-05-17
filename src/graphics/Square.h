#include "Shape.h"
#include "math/Vec2.h"
#include <cstdint>

class Square : public Shape {
public:
    Square(const Vec2& topLeft, float sideLength, uint32_t color)
        : Shape(6), topLeft(topLeft), sideLength(sideLength), color(color) {}
    ~Square() = default;
    Vec2 topLeft;
    float sideLength;
    uint32_t color;
};