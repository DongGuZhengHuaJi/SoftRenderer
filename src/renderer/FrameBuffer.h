#pragma once
#include <sys/types.h>
#include <vector>
#include <cstdint>

class FrameBuffer {
public:
    FrameBuffer(int width, int height);
    ~FrameBuffer();

    void clear(uint32_t color = 0);
    
    void setPixel(int x, int y, uint32_t color);

    const std::vector<uint32_t>& getPixels() const;

    int getWidth() const;
    int getHeight() const;

private:
    int m_width;
    int m_height;
    std::vector<uint32_t> m_pixels;
};