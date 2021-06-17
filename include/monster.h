#ifndef DYN_OBJECT_H
#define DYN_OBJECT_H

#include "object.h"

namespace Rg
{
class Monster : public Object
{
public:
    Monster(Vec2 pos, const s_ptr<Texture>& texture, const s_ptr<Renderer>& renderer,
              int hp, int energy_regen = 50, int vision_radius = 5)
        : Object(pos, texture, renderer, energy_regen)
        , m_hp(hp)
        , m_vision_radius(vision_radius)
    {
    }

    int m_hp;
    int m_vision_radius;
};
}

#endif  // DYN_OBJECT_H
