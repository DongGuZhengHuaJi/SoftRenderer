#pragma once
#include "Point.h"
#include "graphics/Shape.h"
#include <vector>

class Points : public Shape {
public:
    std::vector<Point> m_points;
    Points() = default;
    Points(const std::vector<Point>& points) : m_points(points) {}
    ~Points() override = default;

    void draw(Renderer& renderer) const override;
    void transform(const Mat3& matrix) override;
};
