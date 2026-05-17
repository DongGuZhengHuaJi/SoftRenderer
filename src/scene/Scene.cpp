#include "scene/Scene.h"

Scene::Scene() {
    // Demo: a few lines and circles so the screen isn't empty
    // addLine({Vec2(50, 50), Vec2(200, 150), 0xFFFF0000});
    // addLine({Vec2(200, 150), Vec2(100, 300), 0xFF00FF00});
    // addLine({Vec2(100, 300), Vec2(50, 50), 0xFF0000FF});

    // addCircle({Vec2(400, 200), 80, 0xFFFFFF00});
    // addCircle({Vec2(550, 150), 40, 0xFFFF00FF});
    // addCircle({Vec2(650, 300), 60, 0xFF00FFFF});

    // addTriangle({Vec2(500, 400), Vec2(600, 500), Vec2(400, 500), 0xFFFF8800});
}

Scene::~Scene() {
}

void Scene::addPoint(const Point& point) {
    points.push_back(point);
}

void Scene::addLine(const Line& line) {
    lines.push_back(line);
}

void Scene::addTriangle(const Triangle& triangle) {
    triangles.push_back(triangle);
}

void Scene::addCircle(const Circle& circle) {
    circles.push_back(circle);
}

void Scene::updatePoint(int index, const Point& point) {
    if (index < 0 || index >= points.size()) {
        return;
    }
    points[index] = point;
}

void Scene::updateLine(int index, const Line& line) {
    if (index < 0 || index >= lines.size()) {
        return;
    }
    lines[index] = line;
}

void Scene::updateTriangle(int index, const Triangle& triangle) {
    if (index < 0 || index >= triangles.size()) {
        return;
    }
    triangles[index] = triangle;
}

void Scene::updateCircle(int index, const Circle& circle) {
    if (index < 0 || index >= circles.size()) {
        return;
    }
    circles[index] = circle;
}

void Scene::clear() {
    points.clear();
    lines.clear();
    triangles.clear();
    circles.clear();
}

void Scene::clearPoints() {
    points.clear();
}

void Scene::clearLines() {
    lines.clear();
}

void Scene::clearTriangles() {
    triangles.clear();
}

void Scene::clearCircles() {
    circles.clear();
}
