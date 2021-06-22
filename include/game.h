#ifndef GAME_H
#define GAME_H

#include <memory>
#include <vector>

#include <raylib.h>

#include "common.h"
#include "texture_manager.h"
#include "actions/move.h"
#include "map.h"
#include "monster.h"

namespace Rg
{
class Game
{
public:
    ~Game()
    {
        TextureManager::get().cleanup();
        CloseWindow();
    }

    void init();
    void load_textures();
    void update();
    void handle_input();

    void move_player(Vec2 dir)
    {
        m_player->set_action(std::make_shared<Actions::Move>(dir, m_map));
    }

    s_ptr<Monster> m_player;
    s_ptr<Map::Map> m_map;

    Camera2D m_camera;

    std::array<bool, 350> m_keys;
};

}

#endif  // GAME_H
