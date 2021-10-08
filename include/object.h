#ifndef OBJECT_H
#define OBJECT_H

#include <unordered_map>

#include <raylib.h>

#include "vec2.h"
#include "actions/action.h"
#include "common.h"

namespace Rg
{
class Object
{
public:
    Object(Vec2 pos, const s_ptr <Texture2D> &texture)
        : m_pos(pos)
        , m_texture(texture)
    {
    }
    virtual ~Object() = default;

    virtual void update(Color color)
    {
        render(color);
    }

    Vec2 m_pos;

private:
    void render(Color color);

    s_ptr<Texture2D> m_texture;
};
}

#endif  // OBJECT_H
