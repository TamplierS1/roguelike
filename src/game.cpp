#include "game.h"
#include "log.h"

namespace Rg
{
void Game::init()
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_FULLSCREEN_MODE);
    InitWindow(g_screen_width, g_screen_height, "Roguelike");
    SetTargetFPS(60);

    load_textures();

    m_map = std::make_shared<Map::Map>();

    auto player = std::make_shared<Monster>(
        m_map->m_player_start_position, TextureManager::get().get_texture("player"),
        100, 50, 6);
    m_player = player;

    m_camera.target = (m_player->m_pos * g_tile_size + g_tile_size / 2).to_Vector2();
    m_camera.offset = Vector2{g_screen_width / 2, g_screen_height / 2};
    m_camera.rotation = 0.0f;
    m_camera.zoom = 2.0f;

    m_map->set_player(m_player);
}

void Game::load_textures()
{
    TextureManager::get().load_texture("../res/player.png", "player");
    TextureManager::get().load_texture("../res/rock_wall.png", "rock_wall");
    TextureManager::get().load_texture("../res/rock_floor.png", "rock_floor");
}

void Game::update()
{
    handle_input();

    ClearBackground(BLACK);

    BeginMode2D(m_camera);

    m_map->update();
    m_player->update(WHITE);

    EndMode2D();

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

    m_camera.target = (m_player->m_pos * g_tile_size + g_tile_size / 2).to_Vector2();

    //    m_camera.x = m_player->m_pos.x * g_tile_size + g_tile_size / 2 -
    //                 g_screen_width / 2 / m_camera.zoom;
    //    m_camera.y = m_player->m_pos.y * g_tile_size + g_tile_size / 2 -
    //                 g_screen_height / 2 / m_camera.zoom;
}

void Game::handle_input()
{
    if (m_keys[KEY_D])
        move_player(Vec2{1, 0});
    else if (m_keys[KEY_A])
        move_player(Vec2{-1, 0});
    else if (m_keys[KEY_W])
        move_player(Vec2{0, -1});
    else if (m_keys[KEY_S])
        move_player(Vec2{0, 1});
}

}
