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
        process_events();
        // InputController should always be updated after
        // all events are processed.
        InputController::get().update();

        update_objects();
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

    TextureManager::get().load_texture("../res/texture.png", "krest", m_renderer.get());

    auto object = std::make_shared<Object>(Vec2{64, 64});
    auto render = std::make_shared<Components::Render>(
        TextureManager::get().get_texture("krest"), m_renderer);
    auto control = std::make_shared<Components::Control>();
    auto physics = std::make_shared<Components::Physics>(1);

    object->add_component("render", render);
    object->add_component("control", control);
    object->add_component("physics", physics);

    m_objects.emplace_back(object);
}

void Game::process_events()
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
}

void Game::update_objects()
{
    m_renderer->render_begin();
    for (auto& object : m_objects)
    {
        object->update();
    }
    m_renderer->render_end();
}

}
