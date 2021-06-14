#ifndef COLOR_H
#define COLOR_H

namespace Rg
{
struct Color
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

static constexpr Color g_color_black{0, 0, 0, 255};
static constexpr Color g_color_white{255, 255, 255, 255};

constexpr static bool operator==(const Color& color1, const Color& color2)
{
    return color1.r == color2.r && color1.g == color2.g && color1.b == color2.b &&
           color1.a == color2.a;
}
}

#endif  // COLOR_H
