#include "app/Application.h"
#include "config.h"
#include "graphics/Point.h"
#include "graphics/Line.h"
#include "graphics/Triangle.h"
#include "graphics/Circle.h"
#include "graphics/Rectangle.h"
#include "graphics/Square.h"
#include "graphics/Points.h"
#include <SDL2/SDL_keycode.h>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {
[[noreturn]] void failSdlInit(SDL_Window* window,
                              SDL_Renderer* renderer,
                              SDL_Texture* texture,
                              const char* step) {
    std::string message = std::string(step) + " failed: " + SDL_GetError();

    if (texture) {
        SDL_DestroyTexture(texture);
    }
    if (renderer) {
        SDL_DestroyRenderer(renderer);
    }
    if (window) {
        SDL_DestroyWindow(window);
    }

    SDL_Quit();
    throw std::runtime_error(message);
}
}

Application::Application() : m_frameBuffer(APP_WIDTH, APP_HEIGHT), m_renderer(m_frameBuffer) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        failSdlInit(nullptr, nullptr, nullptr, "SDL_Init");
    }

    m_window = SDL_CreateWindow(
        "SoftRenderer - 2D Drawing",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        APP_WIDTH, APP_HEIGHT,
        SDL_WINDOW_SHOWN
    );
    if (!m_window) {
        failSdlInit(nullptr, nullptr, nullptr, "SDL_CreateWindow");
    }

    m_sdlrenderer = SDL_CreateRenderer(
        m_window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    if (!m_sdlrenderer) {
        failSdlInit(m_window, nullptr, nullptr, "SDL_CreateRenderer");
    }

    m_texture = SDL_CreateTexture(
        m_sdlrenderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        APP_WIDTH, APP_HEIGHT
    );
    if (!m_texture) {
        failSdlInit(m_window, m_sdlrenderer, nullptr, "SDL_CreateTexture");
    }

    m_running = true;

    printHelp();
}

Application::~Application() {
    SDL_DestroyTexture(m_texture);
    SDL_DestroyRenderer(m_sdlrenderer);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

void Application::run() {
    while (m_running) {
        processEvents();
        update();
        render();
    }
}

void Application::processEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            m_running = false;
            break;
        case SDL_KEYDOWN:
            handleKeyDown(event.key);
            break;
        case SDL_MOUSEBUTTONDOWN:
            handleMouseButtonDown(event.button);
            break;
        case SDL_MOUSEMOTION:
            handleMouseMotion(event.motion);
            break;
        }
    }
}

void Application::update() {
}

void Application::render() {
    m_frameBuffer.clear();

    drawScene();
    drawClipOverlay();

    Vec2 mouseWorld = m_renderer.screenToWorld(Vec2(m_mouseX, m_mouseY));

    // Show multi-click previews
    if (m_drawMode == DrawMode::Line && !m_lineFirstClick) {
        m_renderer.drawLine(m_lineStart, mouseWorld, 0x88FFFFFF);
    }
    else if (m_drawMode == DrawMode::Triangle) {
        if (m_triangleClicks == 1) {
            m_renderer.drawLine(m_triangleVertices[0], mouseWorld, 0x88FFFFFF);
        } else if (m_triangleClicks == 2) {
            m_renderer.drawLine(m_triangleVertices[0], m_triangleVertices[1], 0x88FFFFFF);
            m_renderer.drawLine(m_triangleVertices[1], mouseWorld, 0x88FFFFFF);
            m_renderer.drawLine(mouseWorld, m_triangleVertices[0], 0x88FFFFFF);
        }
    }
    else if (m_drawMode == DrawMode::Circle && !m_circleFirstClick) {
        float radius = sqrtf((mouseWorld.x - m_circleCenter.x) * (mouseWorld.x - m_circleCenter.x) +
                             (mouseWorld.y - m_circleCenter.y) * (mouseWorld.y - m_circleCenter.y));
        m_renderer.drawCircle(m_circleCenter, radius, 0x88FFFFFF);
    }
    else if (m_drawMode == DrawMode::Rectangle && !m_rectangleFirstClick) {
        m_renderer.drawRectangle(m_rectangleStart, mouseWorld, 0x88FFFFFF);
    }
    else if (m_drawMode == DrawMode::Square && !m_squareFirstClick) {
        float sideLength = std::max(std::abs(mouseWorld.x - m_squareStart.x), std::abs(mouseWorld.y - m_squareStart.y));
        float tlX = (mouseWorld.x >= m_squareStart.x) ? m_squareStart.x : m_squareStart.x - sideLength;
        float tlY = (mouseWorld.y >= m_squareStart.y) ? m_squareStart.y : m_squareStart.y - sideLength;
        m_renderer.drawSquare(Vec2(tlX, tlY), sideLength, 0x88FFFFFF);
    }
    else if (m_drawMode == DrawMode::ClipRect && !m_clipRectFirstClick) {
        Vec2 clipStartWorld = m_renderer.screenToWorld(m_clipRectStart);
        m_renderer.drawRectangle(clipStartWorld, mouseWorld, 0x8800FF00);
    }
    else if (m_drawMode == DrawMode::ClipPolygon) {
        // Draw preview: existing polygon edges + line to cursor
        // m_clipVertices are in screen space, convert to world for drawing
        auto& verts = m_clipVertices;
        for (size_t i = 1; i < verts.size(); i++) {
            Vec2 a = m_renderer.screenToWorld(verts[i-1]);
            Vec2 b = m_renderer.screenToWorld(verts[i]);
            m_renderer.drawLine(a, b, 0x8800FF00);
        }
        if (!verts.empty()) {
            Vec2 last = m_renderer.screenToWorld(verts.back());
            m_renderer.drawLine(last, mouseWorld, 0x8800FF00);
        }
    }

    SDL_UpdateTexture(
        m_texture,
        nullptr,
        m_frameBuffer.getPixels().data(),
        m_frameBuffer.getWidth() * sizeof(uint32_t)
    );

    SDL_RenderClear(m_sdlrenderer);

    SDL_RenderCopy(
        m_sdlrenderer,
        m_texture,
        nullptr,
        nullptr
    );

    SDL_RenderPresent(m_sdlrenderer);
}

void Application::drawScene() {
    for (const auto& shape : m_scene.m_shapes) {
        shape->draw(m_renderer);
    }
}

void Application::drawClipOverlay() {
    if (!m_renderer.isClipping()) return;

    const uint32_t overlayColor = 0xFFFF0000;

    // Rect clip: draw rectangle outline (clip stored in screen space, convert to world)
    const Rect& r = m_renderer.getClipRect();
    Vec2 tl = m_renderer.screenToWorld(Vec2(r.xMin, r.yMin));
    Vec2 br = m_renderer.screenToWorld(Vec2(r.xMax, r.yMax));
    m_renderer.drawRectangle(tl, br, overlayColor);

    // Polygon clip: draw polygon edges (clip stored in screen space, convert to world)
    const auto& poly = m_renderer.getClipPolygon();
    for (size_t i = 0; i < poly.size(); i++) {
        Vec2 a = m_renderer.screenToWorld(poly[i]);
        Vec2 b = m_renderer.screenToWorld(poly[(i + 1) % poly.size()]);
        m_renderer.drawLine(a, b, overlayColor);
    }
}

void Application::resetMultiClick() {
    m_lineFirstClick = true;
    m_triangleClicks = 0;
    m_circleFirstClick = true;
    m_rectangleFirstClick = true;
    m_squareFirstClick = true;
    m_clipRectFirstClick = true;
    m_clipVertices.clear();
}

void Application::handleKeyDown(const SDL_KeyboardEvent& key) {
    switch (key.keysym.sym) {
    case SDLK_1:
        m_drawMode = DrawMode::Point;
        resetMultiClick();
        printStatus();
        break;
    case SDLK_2:
        m_drawMode = DrawMode::Line;
        resetMultiClick();
        printStatus();
        break;
    case SDLK_3:
        m_drawMode = DrawMode::Triangle;
        resetMultiClick();
        printStatus();
        break;
    case SDLK_4:
        m_drawMode = DrawMode::Circle;
        resetMultiClick();
        printStatus();
        break;
    case SDLK_5:
        m_drawMode = DrawMode::Rectangle;
        resetMultiClick();
        printStatus();
        break;
    case SDLK_6:
        m_drawMode = DrawMode::Square;
        resetMultiClick();
        printStatus();
        break;
    case SDLK_7:
        m_drawMode = DrawMode::Fill;
        resetMultiClick();
        printStatus();
        break;
    case SDLK_8:
        m_drawMode = DrawMode::ClipRect;
        resetMultiClick();
        m_clipRectFirstClick = true;
        printStatus();
        break;
    case SDLK_9:
        m_drawMode = DrawMode::ClipPolygon;
        resetMultiClick();
        m_clipVertices.clear();
        printStatus();
        break;
    case SDLK_0:
        m_renderer.clearClip();
        m_clipVertices.clear();
        m_clipRectFirstClick = true;
        std::cout << "[Clip] Clipping cleared.\n";
        break;
    case SDLK_c:
        m_scene.clear();
        resetMultiClick();
        std::cout << "[Clear] All shapes removed.\n";
        break;
    case SDLK_r:
        m_currentColor = getNextColor();
        std::cout << "[Color] Changed to 0x" << std::hex << m_currentColor << std::dec << "\n";
        break;
    case SDLK_h:
        printHelp();
        break;
    case SDLK_q:
        m_scene.undo();
        break;
    case SDLK_e:
        m_scene.redo();
        break;
    // --- Transforms (apply to all shapes) ---
    case SDLK_UP:
        m_scene.transformAll(Mat3::translate(0, 10));
        std::cout << "[Transform] Translate (0, 10)\n";
        break;
    case SDLK_DOWN:
        m_scene.transformAll(Mat3::translate(0, -10));
        std::cout << "[Transform] Translate (0, -10)\n";
        break;
    case SDLK_LEFT:
        m_scene.transformAll(Mat3::translate(-10, 0));
        std::cout << "[Transform] Translate (-10, 0)\n";
        break;
    case SDLK_RIGHT:
        m_scene.transformAll(Mat3::translate(10, 0));
        std::cout << "[Transform] Translate (10, 0)\n";
        break;
    case SDLK_t:
        m_scene.transformAll(Mat3::scale(1.1f, 1.1f));
        std::cout << "[Transform] Scale 1.1x\n";
        break;
    case SDLK_g:
        m_scene.transformAll(Mat3::scale(0.9f, 0.9f));
        std::cout << "[Transform] Scale 0.9x\n";
        break;
    case SDLK_z:
        m_scene.transformAll(Mat3::rotate(-15.0f * 3.14159265f / 180.0f));
        std::cout << "[Transform] Rotate -15 deg\n";
        break;
    case SDLK_x:
        m_scene.transformAll(Mat3::rotate(15.0f * 3.14159265f / 180.0f));
        std::cout << "[Transform] Rotate +15 deg\n";
        break;
    case SDLK_f:
        m_scene.transformAll(Mat3::reflectX());
        std::cout << "[Transform] Reflect X\n";
        break;
    case SDLK_v:
        m_scene.transformAll(Mat3::reflectY());
        std::cout << "[Transform] Reflect Y\n";
        break;
    case SDLK_b:
        m_scene.transformAll(Mat3::shear(0.15f, 0));
        std::cout << "[Transform] Shear X +0.15\n";
        break;
    case SDLK_n:
        m_scene.transformAll(Mat3::shear(-0.15f, 0));
        std::cout << "[Transform] Shear X -0.15\n";
        break;
    case SDLK_ESCAPE:
        if (m_drawMode == DrawMode::ClipPolygon && !m_clipVertices.empty()) {
            m_clipVertices.clear();
            std::cout << "[Cancel] Clip polygon cancelled.\n";
        } else if (!m_lineFirstClick) {
            m_lineFirstClick = true;
            std::cout << "[Cancel] Line drawing cancelled.\n";
        } else if (m_triangleClicks > 0) {
            m_triangleClicks = 0;
            std::cout << "[Cancel] Triangle drawing cancelled.\n";
        } else if (!m_circleFirstClick) {
            m_circleFirstClick = true;
            std::cout << "[Cancel] Circle drawing cancelled.\n";
        } else if (!m_clipRectFirstClick) {
            m_clipRectFirstClick = true;
            std::cout << "[Cancel] Clip rectangle cancelled.\n";
        }
        break;
    }
}

void Application::handleMouseButtonDown(const SDL_MouseButtonEvent& button) {
    Vec2 pos(button.x, button.y);       // screen coords (for clip)
    Vec2 worldPos = m_renderer.screenToWorld(pos);  // world coords (for shapes)

    // Right-click: finalize polygon clipping
    if (button.button == SDL_BUTTON_RIGHT) {
        if (m_drawMode == DrawMode::ClipPolygon && m_clipVertices.size() >= 3) {
            m_renderer.setClipPolygon(m_clipVertices);
            std::cout << "[Clip] Polygon clip set with " << m_clipVertices.size() << " vertices.\n";
            m_clipVertices.clear();
        }
        return;
    }

    if (button.button != SDL_BUTTON_LEFT) return;

    switch (m_drawMode) {
    case DrawMode::Point:
        m_scene.addPoint(worldPos, m_currentColor);
        std::cout << "[Draw] Point at (" << worldPos.x << ", " << worldPos.y << ")\n";
        break;

    case DrawMode::Line:
        if (m_lineFirstClick) {
            m_lineStart = worldPos;
            m_lineFirstClick = false;
            std::cout << "[Line] Start at (" << worldPos.x << ", " << worldPos.y << "), click end point...\n";
        } else {
            m_scene.addLine(m_lineStart, worldPos, m_currentColor);
            m_lineFirstClick = true;
            std::cout << "[Line] End at (" << worldPos.x << ", " << worldPos.y << ")\n";
        }
        break;

    case DrawMode::Triangle:
        if (m_triangleClicks == 0) {
            m_triangleVertices[0] = worldPos;
            m_triangleClicks = 1;
            std::cout << "[Triangle] V1 at (" << worldPos.x << ", " << worldPos.y << "), click V2...\n";
        } else if (m_triangleClicks == 1) {
            m_triangleVertices[1] = worldPos;
            m_triangleClicks = 2;
            std::cout << "[Triangle] V2 at (" << worldPos.x << ", " << worldPos.y << "), click V3...\n";
        } else {
            m_scene.addTriangle(m_triangleVertices[0], m_triangleVertices[1], worldPos, m_currentColor);
            m_triangleClicks = 0;
            std::cout << "[Triangle] V3 at (" << worldPos.x << ", " << worldPos.y << ")\n";
        }
        break;

    case DrawMode::Circle: {
        if (m_circleFirstClick) {
            m_circleCenter = worldPos;
            m_circleFirstClick = false;
            std::cout << "[Circle] Center at (" << worldPos.x << ", " << worldPos.y << "), click radius point...\n";
        } else {
            float radius = sqrtf((worldPos.x - m_circleCenter.x) * (worldPos.x - m_circleCenter.x) +
                                 (worldPos.y - m_circleCenter.y) * (worldPos.y - m_circleCenter.y));
            m_scene.addCircle(m_circleCenter, radius, m_currentColor);
            m_circleFirstClick = true;
            std::cout << "[Draw] Circle r=" << radius << "\n";
        }
        break;
    }
    case DrawMode::Rectangle: {
        if (m_rectangleFirstClick) {
            m_rectangleStart = worldPos;
            m_rectangleFirstClick = false;
            std::cout << "[Rectangle] Start at (" << worldPos.x << ", " << worldPos.y << "), click opposite corner...\n";
        } else {
            Vec2 topLeft(
                std::min(m_rectangleStart.x, worldPos.x),
                std::min(m_rectangleStart.y, worldPos.y)
            );
            Vec2 bottomRight(
                std::max(m_rectangleStart.x, worldPos.x),
                std::max(m_rectangleStart.y, worldPos.y)
            );
            m_scene.addRectangle(topLeft, bottomRight, m_currentColor);
            m_rectangleFirstClick = true;
            std::cout << "[Rectangle] End at (" << worldPos.x << ", " << worldPos.y << ")\n";
        }
        break;
    }
    case DrawMode::Square: {
        if (m_squareFirstClick) {
            m_squareStart = worldPos;
            m_squareFirstClick = false;
            std::cout << "[Square] Start at (" << worldPos.x << ", " << worldPos.y << "), click opposite corner...\n";
        } else {
            float sideLength = std::max(std::abs(worldPos.x - m_squareStart.x), std::abs(worldPos.y - m_squareStart.y));
            float tlX = (worldPos.x >= m_squareStart.x) ? m_squareStart.x : m_squareStart.x - sideLength;
            float tlY = (worldPos.y >= m_squareStart.y) ? m_squareStart.y : m_squareStart.y - sideLength;
            m_scene.addSquare(Vec2(tlX, tlY), sideLength, m_currentColor);
            m_squareFirstClick = true;
            std::cout << "[Square] side=" << sideLength << "\n";
        }
        break;
    }
    case DrawMode::Fill: {
        Points filledPoints;
        m_renderer.scanlineSeedFill(worldPos, m_currentColor, &filledPoints);
        m_scene.Fill(filledPoints);
        break;
    }
    case DrawMode::ClipRect: {
        // Clip is viewport-space — keep screen coords
        if (m_clipRectFirstClick) {
            m_clipRectStart = pos;
            m_clipRectFirstClick = false;
            std::cout << "[ClipRect] First corner at (" << pos.x << ", " << pos.y << "), click opposite corner...\n";
        } else {
            Rect rect(m_clipRectStart.x, m_clipRectStart.y, pos.x, pos.y);
            m_renderer.setClipRect(rect);
            m_clipRectFirstClick = true;
            std::cout << "[ClipRect] Set: (" << rect.xMin << "," << rect.yMin
                      << ")-(" << rect.xMax << "," << rect.yMax << ")\n";
        }
        break;
    }
    case DrawMode::ClipPolygon: {
        // Clip is viewport-space — keep screen coords
        m_clipVertices.push_back(pos);
        std::cout << "[ClipPolygon] Vertex " << m_clipVertices.size()
                  << " at (" << pos.x << ", " << pos.y << "). "
                  << (m_clipVertices.size() >= 3 ? "Right-click to finish." : "") << "\n";
        break;
    }
    }
}

void Application::handleMouseMotion(const SDL_MouseMotionEvent& motion) {
    m_mouseX = motion.x;
    m_mouseY = motion.y;
}

void Application::printHelp() const {
    std::cout << "\n";
    std::cout << "=== SoftRenderer - 2D Drawing ===\n";
    std::cout << "  Mouse Click      - Place shape\n";
    std::cout << "  1/2/3/4/5/6/7    - Draw: Point / Line / Triangle / Circle / Rectangle / Square / Fill\n";
    std::cout << "  8                - Clip: Rectangle mode (2-click)\n";
    std::cout << "  9                - Clip: Polygon mode (left=vertex, right=finish)\n";
    std::cout << "  0                - Clear clipping\n";
    std::cout << "  R                - Cycle color\n";
    std::cout << "  C                - Clear all shapes\n";
    std::cout << "  Q / E            - Undo / Redo\n";
    std::cout << "  Arrow keys       - Translate all shapes\n";
    std::cout << "  T / G            - Scale up/down all shapes\n";
    std::cout << "  Z / X            - Rotate left/right all shapes\n";
    std::cout << "  F / V            - Reflect X / Y all shapes\n";
    std::cout << "  B / N            - Shear X +/- all shapes\n";
    std::cout << "  ESC              - Cancel current operation\n";
    std::cout << "  H                - Print this help\n";
    std::cout << "  Close window     - Quit\n";
    std::cout << "================================\n\n";
}

void Application::printStatus() const {
    std::cout << "[Mode] " << drawModeName();
    if (m_renderer.isClipping()) {
        std::cout << "  [Clip: ON]";
    }
    std::cout << "\n";
}

const char* Application::drawModeName() const {
    switch (m_drawMode) {
    case DrawMode::Point:       return "Point (1-click)";
    case DrawMode::Line:        return "Line (2-click)";
    case DrawMode::Triangle:    return "Triangle (3-click)";
    case DrawMode::Circle:      return "Circle (2-click)";
    case DrawMode::Rectangle:   return "Rectangle (2-click)";
    case DrawMode::Square:      return "Square (2-click)";
    case DrawMode::Fill:        return "Fill (1-click)";
    case DrawMode::ClipRect:    return "ClipRect (2-click)";
    case DrawMode::ClipPolygon: return "ClipPolygon (L-click add, R-click finish)";
    }
    return "Unknown";
}

uint32_t Application::getNextColor() {
    constexpr uint32_t colors[] = {
        0xFFFF0000, // Red
        0xFF00FF00, // Green
        0xFF0000FF, // Blue
        0xFFFFFF00, // Yellow
        0xFFFF00FF, // Magenta
        0xFF00FFFF, // Cyan
        0xFFFFFFFF, // White
        0xFFFF8800, // Orange
        0xFF8800FF, // Purple
        0xFF00FF88, // Spring green
    };
    constexpr int colorCount = sizeof(colors) / sizeof(colors[0]);
    m_colorIndex = (m_colorIndex + 1) % colorCount;
    return colors[m_colorIndex];
}
