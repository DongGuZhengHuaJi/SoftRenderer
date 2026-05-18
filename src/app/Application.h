#pragma once
#include <SDL2/SDL.h>
#include "renderer/Renderer.h"
#include "renderer/FrameBuffer.h"
#include "scene/Scene.h"

enum class DrawMode {
    Point,
    Line,
    Triangle,
    Circle,
    Rectangle,
    Square,
    Fill
};

class Application {
public:
    Application();
    ~Application();

    void run();
private:
    void processEvents();
    void update();
    void render();
    void drawScene();

    void handleKeyDown(const SDL_KeyboardEvent& key);
    void handleMouseButtonDown(const SDL_MouseButtonEvent& button);
    void handleMouseMotion(const SDL_MouseMotionEvent& motion);

    void printHelp() const;
    void printStatus() const;
    const char* drawModeName() const;

    uint32_t getNextColor();

private:
    bool m_running = false;

    SDL_Window* m_window = nullptr;
    SDL_Renderer* m_sdlrenderer = nullptr;
    SDL_Texture* m_texture = nullptr;

    Renderer m_renderer;
    FrameBuffer m_frameBuffer;

    Scene m_scene;

    DrawMode m_drawMode = DrawMode::Line;
    uint32_t m_currentColor = 0xFFFF0000;
    int m_colorIndex = 0;

    // Multi-click state for line (2 clicks)
    bool m_lineFirstClick = true;
    Vec2 m_lineStart;

    // Multi-click state for triangle (3 clicks)
    int m_triangleClicks = 0;
    Vec2 m_triangleVertices[2];

    // Multi-click state for circle (2 clicks)
    bool m_circleFirstClick = true;
    Vec2 m_circleCenter;

    // Multi-click state for rectangle (2 clicks)
    bool m_rectangleFirstClick = true;
    Vec2 m_rectangleStart;

    // Multi-click state for square (2 clicks)
    bool m_squareFirstClick = true;
    Vec2 m_squareStart;

    // Mouse cursor position
    int m_mouseX = 0;
    int m_mouseY = 0;

};
