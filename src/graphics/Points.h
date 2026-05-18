#pragma once
#include "Point.h"
#include "graphics/Shape.h"
#include <vector>

class Points : public Shape {
public:
    std::vector<Point> m_points;
    Points() : Shape(7) {}
    Points(const std::vector<Point>& points) : Shape(7),m_points(points) {}
    virtual ~Points() {};
};