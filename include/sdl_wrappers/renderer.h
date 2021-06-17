#ifndef RENDERER_H
#define RENDERER_H

#include "SDL2/SDL.h"

#include "object.h"
#include "color.h"

namespace Rg
{
class Window;
class Texture;

struct Camera
{
    int x;
    int y;
    int zoom = 2;
};

class Renderer
{
public:
    Renderer(Window* window, uint32_t flags, Color color = {0, 0, 0, 255});
    ~Renderer();

    void render_begin();
    void render(Object* object, Texture* texture, Camera camera, Color color);
    void render_end();
    // Access the original SDL struct
    [[nodiscard]] constexpr SDL_Renderer* get() const noexcept
    {
        return m_renderer;
    }

private:
    SDL_Renderer* m_renderer;
};
}

#endif  // RENDERER_H
