#include "scene/Scene.h"
#include "graphics/Shape.h"
#include "graphics/Point.h"
#include "graphics/Line.h"
#include "graphics/Triangle.h"
#include "graphics/Circle.h"
#include "graphics/Rectangle.h"
#include "graphics/Square.h"
#include <iostream>

Scene::Scene() {
}

Scene::~Scene() {
}

void Scene::pushShape(std::shared_ptr<Shape> shape) {
    m_shapes.push_back(shape);
    m_undoStack.push(shape);
    while (!m_redoStack.empty()) {
        m_redoStack.pop();
    }
}

void Scene::addPoint(Vec2 position, uint32_t color) {
    pushShape(std::make_shared<Point>(position, color));
}

void Scene::addLine(Vec2 start, Vec2 end, uint32_t color) {
    pushShape(std::make_shared<Line>(start, end, color));
}

void Scene::addTriangle(Vec2 p1, Vec2 p2, Vec2 p3, uint32_t color) {
    pushShape(std::make_shared<Triangle>(p1, p2, p3, color));
}

void Scene::addCircle(Vec2 center, float radius, uint32_t color) {
    pushShape(std::make_shared<Circle>(center, radius, color));
}

void Scene::addRectangle(Vec2 topLeft, Vec2 bottomRight, uint32_t color) {
    pushShape(std::make_shared<Rectangle>(topLeft, bottomRight, color));
}

void Scene::addSquare(Vec2 topLeft, float sideLength, uint32_t color) {
    pushShape(std::make_shared<Square>(topLeft, sideLength, color));
}

void Scene::Fill(Points points, uint32_t color) {
    pushShape(std::make_shared<Points>(points));
}

void Scene::undo() {
    if (m_undoStack.empty()) {
        std::cout << "[Undo] Nothing to undo.\n";
        return;
    }

    auto shape = m_undoStack.top();
    m_undoStack.pop();
    m_redoStack.push(shape);

    for (auto it = m_shapes.rbegin(); it != m_shapes.rend(); ++it) {
        if (*it == shape) {
            m_shapes.erase(std::next(it).base());
            break;
        }
    }
    std::cout << "[Undo] Last shape removed.\n";
}

void Scene::redo() {
    if (m_redoStack.empty()) {
        std::cout << "[Redo] Nothing to redo.\n";
        return;
    }

    auto shape = m_redoStack.top();
    m_redoStack.pop();

    m_shapes.push_back(shape);
    m_undoStack.push(shape);
    std::cout << "[Redo] Shape restored.\n";
}

void Scene::clear() {
    m_shapes.clear();
    while (!m_undoStack.empty()) m_undoStack.pop();
    while (!m_redoStack.empty()) m_redoStack.pop();
}

void Scene::transformAll(const Mat3& matrix) {
    // Approximate uniform scale factor for radius adjustment
    float scaleX = std::sqrt(matrix.m[0] * matrix.m[0] + matrix.m[1] * matrix.m[1]);
    float scaleY = std::sqrt(matrix.m[3] * matrix.m[3] + matrix.m[4] * matrix.m[4]);
    float avgScale = (scaleX + scaleY) * 0.5f;

    for (auto& shape : m_shapes) {
        switch (shape->getType()) {
        case 1: { // Point
            auto p = std::static_pointer_cast<Point>(shape);
            p->position = matrix.transform(p->position);
            break;
        }
        case 2: { // Line
            auto l = std::static_pointer_cast<Line>(shape);
            l->start = matrix.transform(l->start);
            l->end   = matrix.transform(l->end);
            break;
        }
        case 3: { // Triangle
            auto t = std::static_pointer_cast<Triangle>(shape);
            t->v0 = matrix.transform(t->v0);
            t->v1 = matrix.transform(t->v1);
            t->v2 = matrix.transform(t->v2);
            break;
        }
        case 4: { // Circle
            auto c = std::static_pointer_cast<Circle>(shape);
            c->center = matrix.transform(c->center);
            c->radius *= avgScale;
            break;
        }
        case 5: { // Rectangle
            auto r = std::static_pointer_cast<Rectangle>(shape);
            for (int i = 0; i < 4; i++) {
                r->v[i] = matrix.transform(r->v[i]);
            }
            break;
        }
        case 6: { // Square
            auto s = std::static_pointer_cast<Square>(shape);
            for (int i = 0; i < 4; i++) {
                s->v[i] = matrix.transform(s->v[i]);
            }
            break;
        }
        case 7: { // Points (fill)
            auto pts = std::static_pointer_cast<Points>(shape);
            for (auto& pt : pts->m_points) {
                pt.position = matrix.transform(pt.position);
            }
            break;
        }
        }
    }
}
