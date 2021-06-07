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

void Renderer::render_begin()
{
    SDL_RenderClear(m_renderer);
}

void Renderer::render(Object* object, Texture* texture)
{
    auto rect = SDL_Rect{
        .x = object->m_pos.x, .y = object->m_pos.y, .w = g_tile_width, .h = g_tile_width};
    SDL_RenderCopy(m_renderer, texture->get(), nullptr, &rect);
}

void Renderer::render_end()
{
    SDL_RenderPresent(m_renderer);
}

}
