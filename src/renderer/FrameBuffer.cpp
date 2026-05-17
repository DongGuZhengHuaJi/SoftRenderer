#include "renderer/FrameBuffer.h"
#include <cstdint>

FrameBuffer::FrameBuffer(int width, int height) : m_width(width), m_height(height), m_pixels(width * height, 0) {

}

FrameBuffer::~FrameBuffer() {

}

void FrameBuffer::clear(uint32_t color) {
    std::fill(m_pixels.begin(), m_pixels.end(), color);
}

void FrameBuffer::setPixel(int x, int y, uint32_t color) {
    if (x < 0 || x >= m_width || y < 0 || y >= m_height) {
        return;
    }
    m_pixels[y * m_width + x] = color;
}

const std::vector<uint32_t>& FrameBuffer::getPixels() const {
    return m_pixels;
}

int FrameBuffer::getWidth() const {
    return m_width;
}

int FrameBuffer::getHeight() const {
    return m_height;
}