#include "scene/Scene.h"

Scene::Scene() {
    // Initialize scene with some default shapes
    // addPoint({Vec2(100, 100), 0xFFFF0000});
    // addLine({Vec2(150, 150), Vec2(300, 300), 0xFF00FF00});
    // addTriangle({Vec2(400, 100), Vec2(500, 300), Vec2(300, 300), 0xFF0000FF});
    // addCircle({Vec2(600, 400), 50, 0xFFFFFF00});
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