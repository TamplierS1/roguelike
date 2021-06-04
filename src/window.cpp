#include "sdl_wrappers/window.h"

namespace Rg
{
Window::Window(std::string_view name, int w, int h, uint32_t flags)
    : m_name(name)
    , m_width(w)
    , m_height(h)
    , m_flags(flags)
{
    m_window = SDL_CreateWindow(name.data(), SDL_WINDOWPOS_UNDEFINED,
                                SDL_WINDOWPOS_UNDEFINED, w, h, flags);
    if (m_window == nullptr)
    {
        Log::error(SDL_GetError());
        throw std::bad_alloc{};
    }
}
Window::~Window()
{
    SDL_DestroyWindow(m_window);
}

}
