#ifndef GAME_H
#define GAME_H

#include <memory>
#include <vector>

#include "common.h"
#include "sdl_wrappers/window.h"
#include "sdl_wrappers/renderer.h"
#include "sdl_wrappers/texture.h"
#include "texture_manager.h"
#include "actions/move.h"
#include "map.h"

namespace Rg
{

class Game
{
public:
    ~Game()
    {
        SDL_Quit();
        IMG_Quit();
    }

    int run();

private:
    void init();
    void load_textures();
    void process_events();
    void update();
    void handle_input(SDL_Event event);

    void move_player(Vec2 dir)
    {
        m_player->set_action(std::make_shared<Actions::Move>(dir));
    }

    u_ptr<Window> m_window;
    s_ptr<Renderer> m_renderer;
    s_ptr<Object> m_player;

    u_ptr<Map::Map> m_map;

    Camera m_camera = {0, 0};
    bool m_is_running = true;
};

}

#endif  // GAME_H
