#include "game.h"
#include "log.h"

namespace Rg
{
int Game::run()
{
    try
    {
        init();
    }
    catch (std::exception& e)
    {
        return EXIT_FAILURE;
    }

    while (m_is_running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    m_is_running = false;
            }
        }

        m_renderer->render(m_texture.get(), 100, 100);
    }

    return EXIT_SUCCESS;
}

void Game::init()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        Log::error(SDL_GetError());
        throw std::bad_alloc{};
    }

    int img_flags = IMG_INIT_PNG;
    if (!(IMG_Init(img_flags) & img_flags))
    {
        Log::error(IMG_GetError());
        throw std::bad_alloc{};
    }

    m_window = std::make_unique<Window>("Roguelike", g_screen_width, g_screen_height,
                                        SDL_WINDOW_RESIZABLE);
    m_renderer = std::make_shared<Renderer>(m_window.get(), SDL_RENDERER_ACCELERATED);
    m_texture = std::make_unique<Texture>("../res/texture.png", m_renderer.get());
}

}
