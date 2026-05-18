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
    m_frameBuffer.clear(0xFF000000);
}

void Renderer::drawPixel(const Vec2& point, uint32_t color) {
    int px = static_cast<int>(point.x);
    int py = static_cast<int>(point.y);

    if (m_clipEnabled) {
        // Rect bounds check
        if (px < m_clipRect.xMin || px > m_clipRect.xMax ||
            py < m_clipRect.yMin || py > m_clipRect.yMax) {
            return;
        }
        // Polygon inside-test (even-odd rule)
        if (!m_clipPolygon.empty()) {
            bool inside = false;
            size_t n = m_clipPolygon.size();
            for (size_t i = 0, j = n - 1; i < n; j = i++) {
                float xi = m_clipPolygon[i].x, yi = m_clipPolygon[i].y;
                float xj = m_clipPolygon[j].x, yj = m_clipPolygon[j].y;
                if (((yi > py) != (yj > py)) &&
                    (px < (xj - xi) * (py - yi) / (yj - yi) + xi)) {
                    inside = !inside;
                }
            }
            if (!inside) return;
        }
    }

    m_frameBuffer.setPixel(px, py, color);
}

void Renderer::drawLine(const Vec2& start, const Vec2& end, uint32_t color) {
    Vec2 a = start;
    Vec2 b = end;
    bool perPixelClip = false;

    if (m_clipEnabled) {
        if (!Clipper::clipLine(a, b, m_clipRect)) {
            return;
        }
        if (!m_clipPolygon.empty()) {
            perPixelClip = true;
        }
    }

    int x0 = static_cast<int>(a.x);
    int y0 = static_cast<int>(a.y);
    int x1 = static_cast<int>(b.x);
    int y1 = static_cast<int>(b.y);

    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        bool draw = true;
        if (perPixelClip) {
            draw = false;
            int px = x0, py = y0;
            size_t n = m_clipPolygon.size();
            for (size_t i = 0, j = n - 1; i < n; j = i++) {
                float xi = m_clipPolygon[i].x, yi = m_clipPolygon[i].y;
                float xj = m_clipPolygon[j].x, yj = m_clipPolygon[j].y;
                if (((yi > py) != (yj > py)) &&
                    (px < (xj - xi) * (py - yi) / (yj - yi) + xi)) {
                    draw = !draw;
                }
            }
        }
        if (draw) {
            drawPixel(Vec2(x0, y0), color);
        }
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x0 += sx; }
        if (e2 < dx)  { err += dx; y0 += sy; }
    }
}

void Renderer::drawTriangle(const Vec2& v0, const Vec2& v1, const Vec2& v2, uint32_t color) {
    drawLine(v0, v1, color);
    drawLine(v1, v2, color);
    drawLine(v2, v0, color);
}

void Renderer::drawCircle(const Vec2& center, float radius, uint32_t color) {
    int x0 = static_cast<int>(center.x);
    int y0 = static_cast<int>(center.y);
    int r = static_cast<int>(radius);

    int x = r;
    int y = 0;
    int err = 0;

    while (x >= y) {
        int symPts[8][2] = {
            {x0 + x, y0 + y}, {x0 + y, y0 + x},
            {x0 - y, y0 + x}, {x0 - x, y0 + y},
            {x0 - x, y0 - y}, {x0 - y, y0 - x},
            {x0 + y, y0 - x}, {x0 + x, y0 - y}
        };
        for (auto& pt : symPts) {
            drawPixel(Vec2(pt[0], pt[1]), color);
        }

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

        if (m_frameBuffer.getPixel(x, y) != targetColor) {
            continue;
        }

        int left = x;
        while (left >= 0 &&
               m_frameBuffer.getPixel(left, y) == targetColor) {
            left--;
        }
        left++;

        int right = x;
        while (right < m_frameBuffer.getWidth() &&
               m_frameBuffer.getPixel(right, y) == targetColor) {
            right++;
        }
        right--;

        for (int i = left; i <= right; i++) {
            drawPixel(Vec2(i, y), color);
            if (filledPoints) {
                filledPoints->m_points.emplace_back(Vec2(i, y), color);
            }
        }

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
                } else if (!fillable) {
                    insideSpan = false;
                }
            }
        }
    }
}

// --- Clipping ---

void Renderer::setClipRect(const Rect& rect) {
    m_clipEnabled = true;
    m_clipRect = rect;
}

void Renderer::setClipPolygon(const std::vector<Vec2>& vertices) {
    if (vertices.size() < 3) return;
    m_clipEnabled = true;
    m_clipPolygon = vertices;

    // Also compute bounding rect for fast Cohen-Sutherland pre-pass
    float xMin = vertices[0].x, xMax = vertices[0].x;
    float yMin = vertices[0].y, yMax = vertices[0].y;
    for (auto& v : vertices) {
        if (v.x < xMin) xMin = v.x;
        if (v.x > xMax) xMax = v.x;
        if (v.y < yMin) yMin = v.y;
        if (v.y > yMax) yMax = v.y;
    }
    m_clipRect = Rect(xMin, yMin, xMax, yMax);
}

void Renderer::clearClip() {
    m_clipEnabled = false;
    m_clipPolygon.clear();
}

bool Renderer::isClipping() const { 
    return m_clipEnabled; 
}
const Rect& Renderer::getClipRect() const { 
    return m_clipRect; 
}

const std::vector<Vec2>& Renderer::getClipPolygon() const { 
    return m_clipPolygon; 
}
