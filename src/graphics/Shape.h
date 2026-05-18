#pragma once
#include "math/Vec2.h"

class Renderer;

class Shape {
public:
    virtual ~Shape() = default;

    virtual void draw(Renderer& renderer) const = 0;
    virtual void transform(const Mat3& matrix) = 0;
};
