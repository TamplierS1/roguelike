#ifndef GAME_H
#define GAME_H

#include <memory>

#include "common.h"
#include "sdl_wrappers/window.h"
#include "sdl_wrappers/renderer.h"
#include "sdl_wrappers/texture.h"

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

    u_ptr<Window> m_window;
    s_ptr<Renderer> m_renderer;
    u_ptr<Texture> m_texture;

    bool m_is_running = true;
};

}

#endif  // GAME_H
