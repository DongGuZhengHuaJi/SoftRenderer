#pragma once
#include "math/Vec2.h"
#include "graphics/Points.h"
#include <memory>
#include <vector>
#include <stack>
#include <cstdint>

class Shape;

class Scene {
public:
    Scene();
    ~Scene();

    void addPoint(Vec2 position, uint32_t color);
    void addLine(Vec2 start, Vec2 end, uint32_t color);
    void addTriangle(Vec2 p1, Vec2 p2, Vec2 p3, uint32_t color);
    void addCircle(Vec2 center, float radius, uint32_t color);
    void addRectangle(Vec2 topLeft, Vec2 bottomRight, uint32_t color);
    void addSquare(Vec2 topLeft, float sideLength, uint32_t color);

    void Fill(Points points, uint32_t color);

    void undo();
    void redo();

    void clear();

    // Apply a 2D affine transform to all shapes in place
    void transformAll(const Mat3& matrix);

    std::vector<std::shared_ptr<Shape>> m_shapes;

private:
    void pushShape(std::shared_ptr<Shape> shape);

    std::stack<std::shared_ptr<Shape>> m_undoStack;
    std::stack<std::shared_ptr<Shape>> m_redoStack;
};
