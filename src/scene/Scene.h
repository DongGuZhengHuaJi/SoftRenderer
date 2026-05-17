#pragma once
#include "graphics/Point.h"
#include "graphics/Line.h"
#include "graphics/Triangle.h"
#include "graphics/Circle.h"
#include <vector>

class Scene {
public:
    Scene();
    ~Scene();

    void addPoint(const Point& point);
    void addLine(const Line& line);
    void addTriangle(const Triangle& triangle);
    void addCircle(const Circle& circle);

    void updatePoint(int index, const Point& point);
    void updateLine(int index, const Line& line);
    void updateTriangle(int index, const Triangle& triangle);
    void updateCircle(int index, const Circle& circle);

    void clear();
    void clearPoints();
    void clearLines();
    void clearTriangles();
    void clearCircles();
public:
    std::vector<Point> points;
    std::vector<Line> lines;
    std::vector<Triangle> triangles;
    std::vector<Circle> circles;

};