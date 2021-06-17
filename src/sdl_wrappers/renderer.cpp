#include "sdl_wrappers/renderer.h"
#include "sdl_wrappers/window.h"
#include "sdl_wrappers/texture.h"

namespace Rg
{
Renderer::Renderer(Window* window, uint32_t flags, Color color)
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

void Renderer::render(Object* object, Texture* texture, Camera camera, Color color)
{
    SDL_SetTextureColorMod(texture->get(), color.r, color.g, color.b);
    auto rect = SDL_Rect{.x = (object->m_pos.x * g_tile_size - camera.x) * camera.zoom,
                         .y = (object->m_pos.y * g_tile_size - camera.y) * camera.zoom,
                         .w = g_tile_size * camera.zoom,
                         .h = g_tile_size * camera.zoom};
    SDL_RenderCopy(m_renderer, texture->get(), nullptr, &rect);
    SDL_SetTextureColorMod(texture->get(), 255, 255, 255);
}

void Renderer::render_end()
{
    SDL_RenderPresent(m_renderer);
}

}
