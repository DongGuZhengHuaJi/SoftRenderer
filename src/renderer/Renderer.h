#pragma once
#include "renderer/FrameBuffer.h"
#include "math/Vec2.h"
#include "graphics/Points.h"
#include <vector>

class Renderer {
public:
    Renderer(FrameBuffer &frameBuffer);
    ~Renderer();

    void clear();

    // Coordinate conversion: world (origin=center, Y-up) <-> screen (top-left, Y-down)
    Vec2 worldToScreen(const Vec2& world) const;
    Vec2 screenToWorld(const Vec2& screen) const;

    // All draw methods accept WORLD coordinates
    void drawPixel(const Vec2& point,  uint32_t color);
    void drawLine(const Vec2& start, const Vec2& end, uint32_t color);
    void drawTriangle(const Vec2& v0, const Vec2& v1, const Vec2& v2, uint32_t color);
    void drawCircle(const Vec2& center, float radius, uint32_t color);
    void drawRectangle(const Vec2& topLeft, const Vec2& bottomRight, uint32_t color);
    void drawSquare(const Vec2& topLeft, float sideLength, uint32_t color);
    void drawQuad(const Vec2& v0, const Vec2& v1, const Vec2& v2, const Vec2& v3, uint32_t color);

    void scanlineSeedFill(const Vec2& seedPoint, uint32_t color, Points* filledPoints);

    // Clipping (receives SCREEN coordinates — viewport-space)
    void setClipRect(const Rect& rect);
    void setClipPolygon(const std::vector<Vec2>& vertices);
    void clearClip();
    bool isClipping() const;
    const Rect& getClipRect() const;
    const std::vector<Vec2>& getClipPolygon() const;

private:
    // Screen-space pixel write with clip check (internal)
    // Returns true if the pixel was actually drawn (passes clip test)
    bool drawPixelScreen(int x, int y, uint32_t color);
    bool isInsideClip(int x, int y) const;

    FrameBuffer& m_frameBuffer;

    bool          m_clipEnabled = false;
    Rect          m_clipRect;
    std::vector<Vec2> m_clipPolygon;
};
