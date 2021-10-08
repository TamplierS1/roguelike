#ifndef GUI_H
#define GUI_H

#define NK_INCLUDE_DEFAULT_ALLOCATOR
#include "nuklear.h"
#include "raylib.h"

#include "game.h"

namespace Rg
{

constexpr float calc_text_width(nk_handle handle, float height,
                                const char* text, int len);
constexpr Color to_ray_color(const nk_color& color);
constexpr Vector2 to_ray_vec(const struct nk_vec2i& vec);

class GUI
{
public:
    GUI();
    ~GUI();

    int draw();

private:
    void handle_input();
    void render_windows();
    void render();
    void load_fonts();

    Game m_game;
    std::array<bool, 350> m_keys;

    struct nk_context m_nk_ctx;
    nk_user_font m_nk_font;
    u_ptr<Font> m_ray_font;
};
}

#endif  // GUI_H
