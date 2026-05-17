#pragma once
#include <SDL2/SDL.h>
#include "renderer/Renderer.h"
#include "renderer/FrameBuffer.h"
#include "scene/Scene.h"

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
private:
    bool m_running = false;

    SDL_Window* m_window = nullptr;
    SDL_Renderer* m_sdlrenderer = nullptr;
    SDL_Texture* m_texture = nullptr;

    Renderer m_renderer;
    FrameBuffer m_frameBuffer;

    Scene m_scene;

};