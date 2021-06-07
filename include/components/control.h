#ifndef CONTROL_H
#define CONTROL_H

#include "vec2.h"
#include "component.h"
#include "input_controller.h"

namespace Rg::Components
{
class Control : public IComponent
{
public:
    void update(Object* object) override;
    [[nodiscard]] constexpr bool is_moving() const noexcept
    {
        return m_direction.x != 0 || m_direction.y != 0;
    }

    Vec2 m_direction = {0, 0};
};
}

#endif  // CONTROL_H
