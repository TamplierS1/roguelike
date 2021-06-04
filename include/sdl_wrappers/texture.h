#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#include "log.h"

namespace Rg
{
class Renderer;

class Texture
{
public:
    Texture(const std::string& path, Renderer* renderer);
    ~Texture();

    // Access the original SDL struct
    [[nodiscard]] constexpr SDL_Texture* get() const noexcept
    {
        return m_texture;
    }

    int m_width;
    int m_height;

private:
    SDL_Texture* m_texture;
};
}

#endif  // TEXTURE_H
