#include "components/physics.h"
#include "components/control.h"
#include "object.h"

namespace Rg::Components
{
void Physics::update(Object* object)
{
    auto control_component =
        dynamic_cast<Components::Control*>(object->get_component("control"));

    int distance = m_speed * g_tile_width;

    // Calculate the position the object should end up in.
    if (!m_is_moving && control_component->is_moving())
    {
        m_end_position.x = object->m_pos.x + distance * control_component->m_direction.x;
        m_end_position.y = object->m_pos.y + distance * control_component->m_direction.y;

        m_is_moving = true;
    }

    object->m_pos.x += distance * control_component->m_direction.x;
    object->m_pos.y += distance * control_component->m_direction.y;

    if (m_end_position == object->m_pos)
    {
        m_is_moving = false;
    }
}
}
