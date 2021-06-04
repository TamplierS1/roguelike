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

static constexpr int g_screen_width = 1920;
static constexpr int g_screen_height = 1080;

}

#endif  // COMMON_H
