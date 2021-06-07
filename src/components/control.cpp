#include "components/control.h"

namespace Rg::Components
{
void Control::update(Object* object)
{
    auto& input = InputController::get();
    if (input.is_pressed(SDL_SCANCODE_W))
    {
        m_direction.y = -1;
    }
    else if (input.is_pressed(SDL_SCANCODE_S))
    {
        m_direction.y = 1;
    }
    else
    {
        m_direction.y = 0;
    }

    if (input.is_pressed(SDL_SCANCODE_D))
    {
        m_direction.x = 1;
    }
    else if (input.is_pressed(SDL_SCANCODE_A))
    {
        m_direction.x = -1;
    }
    else
    {
        m_direction.x = 0;
    }
}
}
