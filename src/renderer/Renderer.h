#pragma once
#include "renderer/FrameBuffer.h"
#include "math/Vec2.h"

class Renderer {
public:
    Renderer(FrameBuffer &frameBuffer);
    ~Renderer();

    void clear();
    void drawPixel(const Vec2& point,  uint32_t color);
    void drawLine(const Vec2& start, const Vec2& end, uint32_t color);
    void drawTriangle(const Vec2& v0, const Vec2& v1, const Vec2& v2, uint32_t color);
    void drawCircle(const Vec2& center, float radius, uint32_t color);
private:
    FrameBuffer& m_frameBuffer;
};
