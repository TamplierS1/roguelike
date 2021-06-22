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
    Object(Vec2 pos, const s_ptr<Texture2D>& texture, int energy_regen = 0)
        : m_pos(pos)
        , m_texture(texture)
        , m_energy_regen(energy_regen)
    {
    }
    virtual ~Object() = default;

    virtual void update(Color color)
    {
        m_energy += m_energy_regen;
        render(color);
    }

    Vec2 m_pos;
    // Every action consumes energy.
    // Every object gets some amount of energy every turn.
    // Faster actors get more energy. 100 is base for everything.
    int m_energy = 100;

private:
    void render(Color color);

    s_ptr<Texture2D> m_texture;

    int m_energy_regen;
};
}

#endif  // OBJECT_H
