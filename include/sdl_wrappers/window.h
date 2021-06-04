#ifndef WINDOW_H
#define WINDOW_H

#include "SDL2/SDL.h"

#include "log.h"

namespace Rg
{
class Window
{
public:
    explicit Window(std::string_view name, int w = 800, int h = 600,
                    uint32_t flags = SDL_WINDOW_RESIZABLE);
    ~Window();

    // Access the original SDL struct
    [[nodiscard]] constexpr SDL_Window* get() const noexcept
    {
        return m_window;
    }

    const std::string_view m_name;
    const int m_width;
    const int m_height;
    const uint32_t m_flags;

private:
    SDL_Window* m_window;
};
}

#endif  // WINDOW_H
