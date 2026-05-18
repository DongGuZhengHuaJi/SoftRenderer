#include "graphics/Points.h"
#include "renderer/FrameBuffer.h"
#include "scene/Scene.h"
#include "renderer/Renderer.h"
#include <math.h>
#include <memory>
#include <queue>

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

void Renderer::drawRectangle(const Vec2& topLeft, const Vec2& bottomRight, uint32_t color) {
    drawLine(topLeft, Vec2(bottomRight.x, topLeft.y), color);
    drawLine(Vec2(bottomRight.x, topLeft.y), bottomRight, color);
    drawLine(bottomRight, Vec2(topLeft.x, bottomRight.y), color);
    drawLine(Vec2(topLeft.x, bottomRight.y), topLeft, color);
}

void Renderer::drawSquare(const Vec2& topLeft, float sideLength, uint32_t color) {
    Vec2 bottomRight(topLeft.x + sideLength, topLeft.y + sideLength);
    drawRectangle(topLeft, bottomRight, color);
}

void Renderer::scanlineSeedFill(const Vec2& seedPoint, uint32_t color, Points* filledPoints) {

    int seedX = static_cast<int>(seedPoint.x);
    int seedY = static_cast<int>(seedPoint.y);

    if (seedX < 0 || seedX >= m_frameBuffer.getWidth() ||
        seedY < 0 || seedY >= m_frameBuffer.getHeight()) {
        return;
    }

    uint32_t targetColor = m_frameBuffer.getPixel(seedX, seedY);

    if (targetColor == color) {
        return;
    }

    std::queue<Vec2> q;
    q.push(Vec2(seedX, seedY));

    while (!q.empty()) {

        Vec2 p = q.front();
        q.pop();

        int x = static_cast<int>(p.x);
        int y = static_cast<int>(p.y);

        // 如果当前点不是目标颜色，跳过
        if (m_frameBuffer.getPixel(x, y) != targetColor) {
            continue;
        }

        // 1. 找左边界
        int left = x;
        while (left >= 0 &&
               m_frameBuffer.getPixel(left, y) == targetColor) {
            left--;
        }
        left++;

        // 2. 找右边界
        int right = x;
        while (right < m_frameBuffer.getWidth() &&
               m_frameBuffer.getPixel(right, y) == targetColor) {
            right++;
        }
        right--;

        // 3. 填充整条扫描线
        for (int i = left; i <= right; i++) {
            drawPixel(Vec2(i, y), color);
            if (filledPoints) {
                filledPoints->m_points.emplace_back(Vec2(i, y), color);
            }
        }

        // 4. 检查上一行和下一行
        for (int ny : { y - 1, y + 1 }) {

            if (ny < 0 || ny >= m_frameBuffer.getHeight()) {
                continue;
            }

            bool insideSpan = false;

            for (int i = left; i <= right; i++) {

                bool fillable =
                    m_frameBuffer.getPixel(i, ny) == targetColor;

                if (fillable && !insideSpan) {
                    q.push(Vec2(i, ny));
                    insideSpan = true;
                }
                else if (!fillable) {
                    insideSpan = false;
                }
            }
        }
    }
}