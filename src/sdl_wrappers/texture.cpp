#include "sdl_wrappers/texture.h"
#include "sdl_wrappers/renderer.h"

namespace Rg
{
Texture::Texture(const std::string& path, Renderer* renderer)
{
    SDL_Surface* loaded_surface = IMG_Load(path.c_str());
    if (loaded_surface == nullptr)
    {
        Log::error("Failed to load texture " + path + ":\n\t" + IMG_GetError());
        throw std::bad_alloc{};
    }

    m_texture = SDL_CreateTextureFromSurface(renderer->get(), loaded_surface);
    if (m_texture == nullptr)
    {
        Log::error("Failed to create texture " + path + ":\n\t" + SDL_GetError());
        throw std::bad_alloc{};
    }

    // Initialise m_width, m_height.
    SDL_QueryTexture(m_texture, nullptr, nullptr, &m_width, &m_height);

    if (m_width != g_tile_size || m_height != g_tile_size)
    {
        Log::error("Wrong texture size: " + std::to_string(m_width) + " x " + " " +
                   std::to_string(m_height) + ".\n");
    }

    SDL_FreeSurface(loaded_surface);
}
Texture::~Texture()
{
    SDL_DestroyTexture(m_texture);
}

}
