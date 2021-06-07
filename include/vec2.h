#ifndef VEC2_H
#define VEC2_H

namespace Rg
{
struct Vec2
{
    int x = 0;
    int y = 0;
};

constexpr static bool operator==(const Vec2& vec1, const Vec2& vec2)
{
    return vec1.x == vec2.x && vec1.y == vec2.y;
}

}

#endif  // VEC2_H
