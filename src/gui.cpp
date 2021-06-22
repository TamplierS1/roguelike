#include "gui.h"

namespace Rg
{
int GUI::draw()
{
    try
    {
        m_game.init();
    }
    catch (std::exception& e)
    {
        return EXIT_FAILURE;
    }

    while (!WindowShouldClose())
    {
        handle_input();

        BeginDrawing();

        m_game.update();

        draw_gui();

        EndDrawing();
    }

    return EXIT_SUCCESS;
}

void GUI::handle_input()
{
    m_keys.fill(false);

    int key = 0;
    while ((key = GetKeyPressed()) && key != 0)
    {
        m_keys[key] = true;
    }

    m_game.m_keys = m_keys;
}

void GUI::draw_gui()
{
    DrawFPS(m_fps_pos.x, m_fps_pos.y);
    draw_sidebar();
}

void GUI::draw_sidebar()
{
    DrawRectangleLinesEx(m_sidebar, m_sidebar_thickness, m_sidebar_color);
}

}
