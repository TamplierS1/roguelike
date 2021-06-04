#ifndef RENDERER_H
#define RENDERER_H

#include "SDL2/SDL.h"

namespace Rg
{
class Window;
class Texture;

struct Color
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

class Renderer
{
public:
    Renderer(Window* window, uint32_t flags, Color color = {0, 0, 0, 255});
    ~Renderer();

    void render(Texture* texture, int x, int y);
    // Access the original SDL struct
    [[nodiscard]] constexpr SDL_Renderer* get() const noexcept
    {
        return m_renderer;
    }

    Color m_clear_color;

private:
    SDL_Renderer* m_renderer;
};
}

#endif  // RENDERER_H
