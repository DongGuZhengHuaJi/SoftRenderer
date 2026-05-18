#include "graphics/Point.h"
#include "graphics/Line.h"
#include "graphics/Triangle.h"
#include "graphics/Circle.h"
#include "graphics/Rectangle.h"
#include "graphics/Square.h"
#include "graphics/Points.h"
#include "renderer/Renderer.h"
#include <cmath>

void Point::draw(Renderer& renderer) const {
    renderer.drawPixel(position, color);
}

void Point::transform(const Mat3& matrix) {
    position = matrix.transform(position);
}

void Line::draw(Renderer& renderer) const {
    renderer.drawLine(start, end, color);
}

void Line::transform(const Mat3& matrix) {
    start = matrix.transform(start);
    end = matrix.transform(end);
}

void Triangle::draw(Renderer& renderer) const {
    renderer.drawTriangle(v0, v1, v2, color);
}

void Triangle::transform(const Mat3& matrix) {
    v0 = matrix.transform(v0);
    v1 = matrix.transform(v1);
    v2 = matrix.transform(v2);
}

void Circle::draw(Renderer& renderer) const {
    renderer.drawCircle(localToWorld, color);
}

void Circle::transform(const Mat3& matrix) {
    localToWorld = matrix * localToWorld;
}

void Rectangle::draw(Renderer& renderer) const {
    renderer.drawQuad(v[0], v[1], v[2], v[3], color);
}

void Rectangle::transform(const Mat3& matrix) {
    for (Vec2& vertex : v) {
        vertex = matrix.transform(vertex);
    }
}

void Square::draw(Renderer& renderer) const {
    renderer.drawQuad(v[0], v[1], v[2], v[3], color);
}

void Square::transform(const Mat3& matrix) {
    for (Vec2& vertex : v) {
        vertex = matrix.transform(vertex);
    }
}

void Points::draw(Renderer& renderer) const {
    for (const Point& point : m_points) {
        renderer.drawPixel(point.position, point.color);
    }
}

void Points::transform(const Mat3& matrix) {
    for (Point& point : m_points) {
        point.position = matrix.transform(point.position);
    }
}
