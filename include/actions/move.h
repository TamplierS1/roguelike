#ifndef MOVE_H
#define MOVE_H

#include "action.h"

namespace Rg::Actions
{
class Move : public IAction
{
public:
    explicit Move(Vec2 dir)
        : m_dir(dir)
    {
    }

    bool execute(Object* object) override
    {
        if (object->m_energy < static_cast<int>(Costs::Move))
        {
            return false;
        }

        object->m_pos.y += g_tile_size * m_dir.y;
        object->m_pos.x += g_tile_size * m_dir.x;

        object->m_energy -= static_cast<int>(Costs::Move);

        return true;
    }

private:
    Vec2 m_dir;
};
}

#endif  // MOVE_H
