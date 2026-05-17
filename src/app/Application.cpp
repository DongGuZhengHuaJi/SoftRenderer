#include "app/Application.h"
#include "config.h"
#include "graphics/Point.h"
#include "graphics/Line.h"
#include "graphics/Triangle.h"
#include "graphics/Circle.h"
#include "graphics/Rectangle.h"
#include "graphics/Square.h"
#include <SDL2/SDL_keycode.h>
#include <iostream>

Application::Application() : m_frameBuffer(APP_WIDTH, APP_HEIGHT), m_renderer(m_frameBuffer) {

    SDL_Init(SDL_INIT_VIDEO);

    m_window = SDL_CreateWindow(
        "SoftRenderer - 2D Drawing",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        APP_WIDTH, APP_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    m_sdlrenderer = SDL_CreateRenderer(
        m_window,
        -1,
        SDL_RENDERER_ACCELERATED
    );

    m_texture = SDL_CreateTexture(
        m_sdlrenderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        APP_WIDTH, APP_HEIGHT
    );

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

    // Show multi-click previews
    if (m_drawMode == DrawMode::Line && !m_lineFirstClick) {
        m_renderer.drawLine(m_lineStart, Vec2(m_mouseX, m_mouseY), 0x88FFFFFF);
    }
    else if (m_drawMode == DrawMode::Triangle) {
        if (m_triangleClicks == 1) {
            m_renderer.drawLine(m_triangleVertices[0], Vec2(m_mouseX, m_mouseY), 0x88FFFFFF);
        } else if (m_triangleClicks == 2) {
            m_renderer.drawLine(m_triangleVertices[0], m_triangleVertices[1], 0x88FFFFFF);
            m_renderer.drawLine(m_triangleVertices[1], Vec2(m_mouseX, m_mouseY), 0x88FFFFFF);
            m_renderer.drawLine(Vec2(m_mouseX, m_mouseY), m_triangleVertices[0], 0x88FFFFFF);
        }
    }
    else if (m_drawMode == DrawMode::Circle && !m_circleFirstClick) {
        float radius = sqrtf((m_mouseX - m_circleCenter.x) * (m_mouseX - m_circleCenter.x) +
                             (m_mouseY - m_circleCenter.y) * (m_mouseY - m_circleCenter.y));
        m_renderer.drawCircle(m_circleCenter, radius, 0x88FFFFFF);
    }
    else if (m_drawMode == DrawMode::Rectangle && !m_rectangleFirstClick) {
        m_renderer.drawRectangle(m_rectangleStart, Vec2(m_mouseX, m_mouseY), 0x88FFFFFF);
    }
    else if (m_drawMode == DrawMode::Square && !m_squareFirstClick) {
        float sideLength = std::max(std::abs(m_mouseX - m_squareStart.x), std::abs(m_mouseY - m_squareStart.y));
        float tlX = (m_mouseX >= m_squareStart.x) ? m_squareStart.x : m_squareStart.x - sideLength;
        float tlY = (m_mouseY >= m_squareStart.y) ? m_squareStart.y : m_squareStart.y - sideLength;
        m_renderer.drawSquare(Vec2(tlX, tlY), sideLength, 0x88FFFFFF);
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
        switch (shape->getType()) {
            case 1: { // Point
                auto p = std::static_pointer_cast<Point>(shape);
                m_renderer.drawPixel(p->position, p->color);
                break;
            }
            case 2: { // Line
                auto l = std::static_pointer_cast<Line>(shape);
                m_renderer.drawLine(l->start, l->end, l->color);
                break;
            }
            case 3: { // Triangle
                auto t = std::static_pointer_cast<Triangle>(shape);
                m_renderer.drawTriangle(t->v0, t->v1, t->v2, t->color);
                break;
            }
            case 4: { // Circle
                auto c = std::static_pointer_cast<Circle>(shape);
                m_renderer.drawCircle(c->center, c->radius, c->color);
                break;
            }
            case 5: { // Rectangle
                auto r = std::static_pointer_cast<Rectangle>(shape);
                m_renderer.drawRectangle(r->topLeft, r->bottomRight, r->color);
                break;
            }
            case 6: { // Square
                auto s = std::static_pointer_cast<Square>(shape);
                m_renderer.drawSquare(s->topLeft, s->sideLength, s->color);
                break;
            }
        }
    }
}

void Application::handleKeyDown(const SDL_KeyboardEvent& key) {
    switch (key.keysym.sym) {
    case SDLK_1:
        m_drawMode = DrawMode::Point;
        m_lineFirstClick = true;
        m_triangleClicks = 0;
        m_circleFirstClick = true;
        printStatus();
        break;
    case SDLK_2:
        m_drawMode = DrawMode::Line;
        m_lineFirstClick = true;
        m_triangleClicks = 0;
        m_circleFirstClick = true;
        printStatus();
        break;
    case SDLK_3:
        m_drawMode = DrawMode::Triangle;
        m_lineFirstClick = true;
        m_triangleClicks = 0;
        m_circleFirstClick = true;
        printStatus();
        break;
    case SDLK_4:
        m_drawMode = DrawMode::Circle;
        m_lineFirstClick = true;
        m_triangleClicks = 0;
        m_circleFirstClick = true;
        printStatus();
        break;
    case SDLK_5:
        m_drawMode = DrawMode::Rectangle;
        m_lineFirstClick = true;
        m_triangleClicks = 0;
        m_circleFirstClick = true;
        printStatus();
        break;
    case SDLK_6:
        m_drawMode = DrawMode::Square;
        m_lineFirstClick = true;
        m_triangleClicks = 0;
        m_circleFirstClick = true;
        printStatus();
        break;
    case SDLK_c:
        m_scene.clear();
        m_lineFirstClick = true;
        m_triangleClicks = 0;
        m_circleFirstClick = true;
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
    case SDLK_ESCAPE:
        if (!m_lineFirstClick) {
            m_lineFirstClick = true;
            std::cout << "[Cancel] Line drawing cancelled.\n";
        } else if (m_triangleClicks > 0) {
            m_triangleClicks = 0;
            std::cout << "[Cancel] Triangle drawing cancelled.\n";
        }
        else if (!m_circleFirstClick) {
            m_circleFirstClick = true;
            std::cout << "[Cancel] Circle drawing cancelled.\n";
        }
        break;
    }
}

void Application::handleMouseButtonDown(const SDL_MouseButtonEvent& button) {
    if (button.button != SDL_BUTTON_LEFT) return;

    Vec2 pos(button.x, button.y);

    switch (m_drawMode) {
    case DrawMode::Point:
        m_scene.addPoint(pos, m_currentColor);
        std::cout << "[Draw] Point at (" << pos.x << ", " << pos.y << ")\n";
        break;

    case DrawMode::Line:
        if (m_lineFirstClick) {
            m_lineStart = pos;
            m_lineFirstClick = false;
            std::cout << "[Line] Start at (" << pos.x << ", " << pos.y << "), click end point...\n";
        } else {
            m_scene.addLine(m_lineStart, pos, m_currentColor);
            m_lineFirstClick = true;
            std::cout << "[Line] End at (" << pos.x << ", " << pos.y << ")\n";
        }
        break;

    case DrawMode::Triangle:
        if (m_triangleClicks == 0) {
            m_triangleVertices[0] = pos;
            m_triangleClicks = 1;
            std::cout << "[Triangle] V1 at (" << pos.x << ", " << pos.y << "), click V2...\n";
        } else if (m_triangleClicks == 1) {
            m_triangleVertices[1] = pos;
            m_triangleClicks = 2;
            std::cout << "[Triangle] V2 at (" << pos.x << ", " << pos.y << "), click V3...\n";
        } else {
            m_scene.addTriangle(m_triangleVertices[0], m_triangleVertices[1], pos, m_currentColor);
            m_triangleClicks = 0;
            std::cout << "[Triangle] V3 at (" << pos.x << ", " << pos.y << ")\n";
        }
        break;

    case DrawMode::Circle: {
        if (m_circleFirstClick) {
            m_circleCenter = pos;
            m_circleFirstClick = false;
            std::cout << "[Circle] Center at (" << pos.x << ", " << pos.y << "), click radius point...\n";
        } else {
            float radius = sqrtf((pos.x - m_circleCenter.x) * (pos.x - m_circleCenter.x) +
                                 (pos.y - m_circleCenter.y) * (pos.y - m_circleCenter.y));
            m_scene.addCircle(m_circleCenter, radius, m_currentColor);
            m_circleFirstClick = true;
            std::cout << "[Draw] Circle at (" << pos.x << ", " << pos.y << ") r=" << radius << "\n";
        }
        break;
    }
    case DrawMode::Rectangle: {
        if (m_rectangleFirstClick) {
            m_rectangleStart = pos;
            m_rectangleFirstClick = false;
            std::cout << "[Rectangle] Start at (" << pos.x << ", " << pos.y << "), click opposite corner...\n";
        } else {
            Vec2 topLeft(
                std::min(m_rectangleStart.x, pos.x),
                std::min(m_rectangleStart.y, pos.y)
            );
            Vec2 bottomRight(
                std::max(m_rectangleStart.x, pos.x),
                std::max(m_rectangleStart.y, pos.y)
            );
            m_scene.addRectangle(topLeft, bottomRight, m_currentColor);
            m_rectangleFirstClick = true;
            std::cout << "[Rectangle] End at (" << pos.x << ", " << pos.y << ")\n";
        }
        break;
    }
    case DrawMode::Square: {
        if (m_squareFirstClick) {
            m_squareStart = pos;
            m_squareFirstClick = false;
            std::cout << "[Square] Start at (" << pos.x << ", " << pos.y << "), click opposite corner...\n";
        } else {
            float sideLength = std::max(std::abs(pos.x - m_squareStart.x), std::abs(pos.y - m_squareStart.y));
            float tlX = (pos.x >= m_squareStart.x) ? m_squareStart.x : m_squareStart.x - sideLength;
            float tlY = (pos.y >= m_squareStart.y) ? m_squareStart.y : m_squareStart.y - sideLength;
            m_scene.addSquare(Vec2(tlX, tlY), sideLength, m_currentColor);
            m_squareFirstClick = true;
            std::cout << "[Square] End at (" << pos.x << ", " << pos.y << ") side=" << sideLength << "\n";
        }
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
    std::cout << "  1/2/3/4/5/6      - Draw: Point / Line / Triangle / Circle / Rectangle / Square\n";
    std::cout << "  R                - Cycle color\n";
    std::cout << "  C                - Clear all shapes\n";
    std::cout << "  Q                - Undo last step\n";
    std::cout << "  E                - Redo last step\n";
    std::cout << "  ESC              - Cancel current line/triangle/circle\n";
    std::cout << "  H                - Toggle this HUD\n";
    std::cout << "  Close window     - Quit\n";
    std::cout << "================================\n\n";
}

void Application::printStatus() const {
    std::cout << "[Mode] " << drawModeName() << "\n";
}

const char* Application::drawModeName() const {
    switch (m_drawMode) {
    case DrawMode::Point:    return "Point (1-click)";
    case DrawMode::Line:     return "Line (2-click)";
    case DrawMode::Triangle: return "Triangle (3-click)";
    case DrawMode::Circle:   return "Circle (1-click)";
    case DrawMode::Rectangle: return "Rectangle (2-click)";
    case DrawMode::Square:   return "Square (2-click)";
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
