#include "app/Application.h"
#include "config.h"

Application::Application() : m_frameBuffer(APP_WIDTH, APP_HEIGHT), m_renderer(m_frameBuffer) {

    SDL_Init(SDL_INIT_VIDEO);

    m_window = SDL_CreateWindow(
        "SoftRenderer",
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
        if (event.type == SDL_QUIT) {
            m_running = false;
        }
    }
}

void Application::update()
{

}

void Application::render() {
    m_frameBuffer.clear();

    drawScene();

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
    if (m_scene.points.size() > 0) {
        for (const auto& point : m_scene.points) {
            m_renderer.drawPixel(point.position, point.color);
        }
    }

    if (m_scene.lines.size() > 0) {
        for (const auto& line : m_scene.lines) {
            m_renderer.drawLine(line.start, line.end, line.color);
        }
    }

    if (m_scene.triangles.size() > 0) {
        for (const auto& triangle : m_scene.triangles) {
            m_renderer.drawTriangle(triangle.v0, triangle.v1, triangle.v2, triangle.color);
        }
    }

    if (m_scene.circles.size() > 0) {
        for (const auto& circle : m_scene.circles) {
            m_renderer.drawCircle(circle.center, circle.radius, circle.color);
        }
    }
}