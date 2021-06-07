#ifndef PHYSICS_H
#define PHYSICS_H

#include "component.h"
#include "vec2.h"

namespace Rg::Components
{
class Physics : public IComponent
{
public:
    explicit Physics(int speed)
        : m_speed(speed)
    {
    }

    void update(Object* object) override;

    // Speed is measured in tiles.
    int m_speed = 0;

private:
    bool m_is_moving = false;
    Vec2 m_end_position = {0, 0};
};
}

#endif  // PHYSICS_H
