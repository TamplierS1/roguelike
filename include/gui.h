#ifndef GUI_H
#define GUI_H

#include "game.h"

namespace Rg
{
class GUI
{
public:
    int draw();

private:
    void handle_input();

    void draw_gui();
    void draw_sidebar();

    Game m_game;
    std::array<bool, 350> m_keys;

    Vec2 m_fps_pos = {20, 20};

    Vec2 m_sidebar_size = {500, g_screen_height - 1};
    Vec2 m_sidebar_pos = {g_screen_width - m_sidebar_size.x, 1};
    Rectangle m_sidebar = {m_sidebar_pos.x, m_sidebar_pos.y, m_sidebar_size.x,
                           m_sidebar_size.y};
    int m_sidebar_thickness = 4;
    Color m_sidebar_color = RAYWHITE;
};
}

#endif  // GUI_H
