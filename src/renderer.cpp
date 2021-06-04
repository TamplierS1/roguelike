#include "sdl_wrappers/renderer.h"
#include "sdl_wrappers/window.h"
#include "sdl_wrappers/texture.h"

namespace Rg
{
Renderer::Renderer(Window* window, uint32_t flags, Color color)
: m_clear_color(color)
{
    m_renderer = SDL_CreateRenderer(window->get(), -1, flags);
    if (m_renderer == nullptr)
    {
        Log::error(SDL_GetError());
        throw std::bad_alloc{};
    }

    SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
}
Renderer::~Renderer()
{
    SDL_DestroyRenderer(m_renderer);
}

void Renderer::render(Texture* texture, int x, int y)
{
    SDL_RenderClear(m_renderer);

    auto rect = SDL_Rect{.x = x, .y = y, .w = texture->m_width, .h = texture->m_height};
    SDL_RenderCopy(m_renderer, texture->get(), nullptr, &rect);

    SDL_RenderPresent(m_renderer);
}

}
