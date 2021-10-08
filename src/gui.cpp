#include <fmt/core.h>

#include "gui.h"
#include "log.h"
#include "common.h"

namespace Rg
{
constexpr float calc_text_width(nk_handle handle, float height, const char* text, int len)
{
    return MeasureTextEx(*(Font*)handle.ptr, text, height, 1).x;
}

constexpr Color to_ray_color(const nk_color& color)
{
    return {color.r, color.g, color.b, color.a};
}

constexpr Vector2 to_ray_vec(const struct nk_vec2i& vec)
{
    return {static_cast<float>(vec.x), static_cast<float>(vec.y)};
}

GUI::GUI()
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_FULLSCREEN_MODE);
    InitWindow(g_screen_width, g_screen_height, "Roguelike");
    SetTargetFPS(60);

    load_fonts();
    if (nk_init_default(&m_nk_ctx, &m_nk_font) == 0)
    {
        Log::error("Failed to load the font.");
        // TODO: Throw and exception and catch it in main.cpp
    }
}

GUI::~GUI()
{
    CloseWindow();

    nk_free(&m_nk_ctx);
    UnloadFont(*m_ray_font);
}

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
        render_windows();
        render();

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

void GUI::render_windows()
{
    auto flags = static_cast<nk_panel_flags>(NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR);

    if (nk_begin(&m_nk_ctx, "World", {0, 0, 200, 80}, flags))
    {
        nk_layout_row_dynamic(&m_nk_ctx, 60, 1);

        std::string player_hp = fmt::format("HP: {}", m_game.m_player->m_hp);
        nk_text(&m_nk_ctx, player_hp.c_str(), 100, NK_TEXT_CENTERED);
    }
    nk_end(&m_nk_ctx);
}

void GUI::render()
{
    const struct nk_command* cmd = 0;
    nk_foreach(cmd, &m_nk_ctx) switch (cmd->type)
    {
        case NK_COMMAND_NOP:
            break;
        case NK_COMMAND_SCISSOR:
            break;
        case NK_COMMAND_LINE:
        {
            // I can't use dynamic_cast, because commands are all C structs :(.
            auto c = reinterpret_cast<const nk_command_line*>(cmd);
            DrawLineEx(to_ray_vec(c->begin), to_ray_vec(c->end), c->line_thickness,
                       to_ray_color(c->color));
            break;
        }
        case NK_COMMAND_CURVE:
            break;
        case NK_COMMAND_RECT:
        {
            auto c = reinterpret_cast<const nk_command_rect*>(cmd);
            Rectangle rec{c->x, c->y, c->w, c->h};
            DrawRectangleLinesEx(rec, c->line_thickness, to_ray_color(c->color));
            break;
        }
        case NK_COMMAND_RECT_FILLED:
        {
            auto c = reinterpret_cast<const nk_command_rect_filled*>(cmd);
            DrawRectangleV(to_ray_vec({c->x, c->y}), to_ray_vec({c->w, c->h}),
                           to_ray_color(c->color));
            break;
        }
        case NK_COMMAND_RECT_MULTI_COLOR:
            break;
        case NK_COMMAND_CIRCLE:
            break;
        case NK_COMMAND_CIRCLE_FILLED:
            break;
        case NK_COMMAND_ARC:
            break;
        case NK_COMMAND_ARC_FILLED:
            break;
        case NK_COMMAND_TRIANGLE:
            break;
        case NK_COMMAND_TRIANGLE_FILLED:
            break;
        case NK_COMMAND_POLYGON:
            break;
        case NK_COMMAND_POLYGON_FILLED:
            break;
        case NK_COMMAND_POLYLINE:
            break;
        case NK_COMMAND_TEXT:
        {
            // I can't use dynamic_cast, because commands are all C structs :(.
            auto c = reinterpret_cast<const nk_command_text*>(cmd);
            DrawTextEx(*m_ray_font, c->string, {c->x, c->y}, c->height, 2,
                       to_ray_color(c->foreground));
            break;
        }
        case NK_COMMAND_IMAGE:
            break;
        case NK_COMMAND_CUSTOM:
            break;
    }

    nk_clear(&m_nk_ctx);
}

void GUI::load_fonts()
{
    m_ray_font =
        std::make_unique<Font>(LoadFontEx("res/Roboto-Light.ttf", 32, nullptr, 250));

    m_nk_font.userdata.ptr = m_ray_font.get();
    m_nk_font.height = m_ray_font->baseSize;
    m_nk_font.width = calc_text_width;
}

}
