#ifndef VEC2_H
#define VEC2_H

namespace Rg
{
struct Vec2
{
    int x = 0;
    int y = 0;

    constexpr Vector2 to_Vector2()
    {
        return Vector2{x, y};
    }
};

constexpr static bool operator==(const Vec2& vec1, const Vec2& vec2)
{
    return vec1.x == vec2.x && vec1.y == vec2.y;
}

constexpr static bool operator>(const Vec2& vec1, const Vec2& vec2)
{
    return vec1.x > vec2.x && vec1.y > vec2.y;
}

constexpr static bool operator>=(const Vec2& vec1, const Vec2& vec2)
{
    return vec1.x >= vec2.x && vec1.y >= vec2.y;
}

constexpr static bool operator<=(const Vec2& vec1, const Vec2& vec2)
{
    return vec1.x <= vec2.x && vec1.y <= vec2.y;
}

constexpr static bool operator<(const Vec2& vec1, const Vec2& vec2)
{
    return vec1.x < vec2.x && vec1.y < vec2.y;
}

constexpr static Vec2 operator-(const Vec2& vec1, int num)
{
    return Vec2{vec1.x - num, vec1.y - num};
}

constexpr static Vec2 operator-(const Vec2& vec1, const Vec2& vec2)
{
    return Vec2{vec1.x - vec2.x, vec1.y - vec2.y};
}

constexpr static Vec2 operator+(const Vec2& vec1, int num)
{
    return Vec2{vec1.x + num, vec1.y + num};
}

constexpr static Vec2 operator+(const Vec2& vec1, const Vec2& vec2)
{
    return Vec2{vec1.x + vec2.x, vec1.y + vec2.y};
}

constexpr static Vec2 operator/(const Vec2& vec1, const Vec2& vec2)
{
    return Vec2{vec1.x / vec2.x, vec1.y / vec2.y};
}

constexpr static Vec2 operator/(const Vec2& vec1, const int number)
{
    return Vec2{vec1.x / number, vec1.y / number};
}

constexpr static Vec2 operator*(const Vec2& vec1, const int number)
{
    return Vec2{vec1.x * number, vec1.y * number};
}
}

#endif  // VEC2_H
