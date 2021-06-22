#ifndef DYN_OBJECT_H
#define DYN_OBJECT_H

#include "object.h"

namespace Rg
{
class Monster : public Object
{
public:
    Monster(Vec2 pos, const s_ptr<Texture>& texture, int hp, int energy_regen = 50,
            int vision_radius = 5)
        : Object(pos, texture, energy_regen)
        , m_hp(hp)
        , m_vision_radius(vision_radius)
    {
    }

    [[nodiscard]] Actions::IAction* get_action()
    {
        auto action = m_current_action;
        m_current_action.reset();
        return action.get();
    }
    void set_action(const s_ptr<Actions::IAction>& action)
    {
        m_current_action = action;
    }

    int m_hp;
    int m_vision_radius;

private:
    s_ptr<Actions::IAction> m_current_action;
};
}

#endif  // DYN_OBJECT_H
