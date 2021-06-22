#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <memory>
#include <exception>

namespace Rg
{
template <class T>
using u_ptr = std::unique_ptr<T>;

template <class T>
using s_ptr = std::shared_ptr<T>;

template <class T>
using w_ptr = std::weak_ptr<T>;

static constexpr int g_screen_width = 1920;
static constexpr int g_screen_height = 1080;

static constexpr int g_tile_size = 16;

constexpr static bool operator==(const Color& color1, const Color& color2)
{
    return color1.r == color2.r && color1.g == color2.g && color1.b == color2.b;
}

}

#endif  // COMMON_H
