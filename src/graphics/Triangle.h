#include "math/Vec2.h"
#include <cstdint>
#include "Shape.h"

class Triangle : public Shape {
public:
    Triangle(const Vec2& v0, const Vec2& v1, const Vec2& v2, uint32_t color)
        : Shape(3), v0(v0), v1(v1), v2(v2), color(color) {
    }    
    ~Triangle() = default;
    Vec2 v0;
    Vec2 v1;
    Vec2 v2;
    uint32_t color;
};