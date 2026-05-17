#include "renderer/FrameBuffer.h"
#include "renderer/Renderer.h"
#include <math.h>

Renderer::Renderer(FrameBuffer &frameBuffer) : m_frameBuffer(frameBuffer) {

}

Renderer::~Renderer() {

}

void Renderer::clear() {
    m_frameBuffer.clear(0xFF000000); // Clear with black color
}

void Renderer::drawPixel(const Vec2& point, uint32_t color) {
    m_frameBuffer.setPixel(static_cast<int>(point.x), static_cast<int>(point.y), color);
}

void Renderer::drawLine(const Vec2& start, const Vec2& end, uint32_t color) {
    // Bresenham's line algorithm
    int x0 = static_cast<int>(start.x);
    int y0 = static_cast<int>(start.y);
    int x1 = static_cast<int>(end.x);
    int y1 = static_cast<int>(end.y);

    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        drawPixel(Vec2(x0, y0), color);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void Renderer::drawTriangle(const Vec2& v0, const Vec2& v1, const Vec2& v2, uint32_t color) {
    drawLine(v0, v1, color);
    drawLine(v1, v2, color);
    drawLine(v2, v0, color);
}

void Renderer::drawCircle(const Vec2& center, float radius, uint32_t color) {
    // Midpoint circle algorithm
    int x0 = static_cast<int>(center.x);
    int y0 = static_cast<int>(center.y);
    int r = static_cast<int>(radius);

    int x = r;
    int y = 0;
    int err = 0;

    while (x >= y) {
        drawPixel(Vec2(x0 + x, y0 + y), color);
        drawPixel(Vec2(x0 + y, y0 + x), color);
        drawPixel(Vec2(x0 - y, y0 + x), color);
        drawPixel(Vec2(x0 - x, y0 + y), color);
        drawPixel(Vec2(x0 - x, y0 - y), color);
        drawPixel(Vec2(x0 - y, y0 - x), color);
        drawPixel(Vec2(x0 + y, y0 - x), color);
        drawPixel(Vec2(x0 + x, y0 - y), color);

        if (err <= 0) {
            err += 2 * y + 1;
            y++;
        } else {
            err -= 2 * x - 1;
            x--;
        }
    }
}