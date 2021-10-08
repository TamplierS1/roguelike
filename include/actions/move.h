#ifndef MOVE_H
#define MOVE_H

#include <typeinfo>

#include "action.h"
#include "monster.h"
#include "map.h"

namespace Rg::Actions
{
class Move : public IAction
{
public:
    explicit Move(Vec2 dir, const s_ptr<Map::Map>& map)
        : m_dir(dir)
        , m_map(map)
    {
    }

    void execute(Object* object) override
    {
        assert(typeid(*object) == typeid(Monster));

        Vec2 new_pos{object->m_pos.x + m_dir.x, object->m_pos.y + m_dir.y};

        if (!m_map.lock()->is_cell_available(new_pos))
        {
            throw ActionFailed{FailReason::SpaceIsOccupied,
                               "Can't move into an occupied space."};
        }

        object->m_pos = new_pos;
    }

private:
    Vec2 m_dir;
    w_ptr<Map::Map> m_map;
};
}

#endif  // MOVE_H
