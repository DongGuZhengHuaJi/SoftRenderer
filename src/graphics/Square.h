#include "Shape.h"
#include "math/Vec2.h"
#include <cstdint>

class Square : public Shape {
public:
    Square(const Vec2& topLeft, float sideLength, uint32_t color)
        : Shape(6), color(color) {
        v[0] = topLeft;
        v[1] = Vec2(topLeft.x + sideLength, topLeft.y);
        v[2] = Vec2(topLeft.x + sideLength, topLeft.y + sideLength);
        v[3] = Vec2(topLeft.x, topLeft.y + sideLength);
    }
    ~Square() = default;
    Vec2 v[4];   // 4 corner vertices in order
    uint32_t color;
};