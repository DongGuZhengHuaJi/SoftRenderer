#include "math/Vec2.h"
#include <cstdint>
#include "Shape.h"

class Circle : public Shape {
public:
    Circle(const Vec2& center, float radius, uint32_t color)
        : Shape(4), center(center), radius(radius), color(color) {}
    ~Circle() = default;
    Vec2 center;
    float radius;
    uint32_t color;
};