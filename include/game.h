#ifndef GAME_H
#define GAME_H

#include <memory>
#include <vector>

#include "common.h"
#include "sdl_wrappers/window.h"
#include "sdl_wrappers/renderer.h"
#include "sdl_wrappers/texture.h"
#include "components/render.h"
#include "components/control.h"
#include "components/physics.h"
#include "texture_manager.h"

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
    void process_events();
    void update_objects();

    u_ptr<Window> m_window;
    s_ptr<Renderer> m_renderer;

    std::vector<s_ptr<Object>> m_objects;

    bool m_is_running = true;
};

}

#endif  // GAME_H
