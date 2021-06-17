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

        update();
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

    load_textures();

    m_map = std::make_shared<Map::Map>(m_renderer);

    auto player = std::make_shared<Monster>(
        m_map->m_player_start_position, TextureManager::get().get_texture("kriegsman"),
        m_renderer, 100, 50, 6);
    m_player = player;

    m_map->set_player(player);
}

void Game::load_textures()
{
    TextureManager::get().load_texture("../res/kriegsman.png", "kriegsman",
                                       m_renderer.get());
    TextureManager::get().load_texture("../res/rock_wall.png", "rock_wall",
                                       m_renderer.get());
    TextureManager::get().load_texture("../res/rock_floor.png", "rock_floor",
                                       m_renderer.get());
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
                break;
            case SDL_KEYDOWN:
                handle_input(event);
                break;
        }
    }
}

void Game::update()
{
    m_renderer->render_begin();

    m_map->update(m_camera);

    m_player->update(m_camera, g_color_white);

    auto action = m_player->get_action();
    if (action != nullptr)
    {
        try
        {
            action->execute(m_player.get());
        }
        catch (const Actions::ActionFailed& e)
        {
            Log::warning(e.m_msg);
        }
    }

    m_camera.x = m_player->m_pos.x * g_tile_size + g_tile_size / 2 -
                 g_screen_width / 2 / m_camera.zoom;
    m_camera.y = m_player->m_pos.y * g_tile_size + g_tile_size / 2 -
                 g_screen_height / 2 / m_camera.zoom;

    m_renderer->render_end();
}

void Game::handle_input(SDL_Event event)
{
    switch (event.key.keysym.sym)
    {
        case SDLK_d:
            move_player(Vec2{1, 0});
            break;
        case SDLK_a:
            move_player(Vec2{-1, 0});
            break;
        case SDLK_w:
            move_player(Vec2{0, -1});
            break;
        case SDLK_s:
            move_player(Vec2{0, 1});
            break;
    }
}

}
