#ifndef INPUT_CONTROLLER_H
#define INPUT_CONTROLLER_H

#include <cstdint>

#include "SDL2/SDL.h"

namespace Rg
{
class InputController
{
public:
    InputController(const InputController&) = delete;
    InputController(InputController&&) = delete;

    void update()
    {
        m_key_states = SDL_GetKeyboardState(nullptr);
    }
    bool is_pressed(SDL_Scancode key)
    {
        return m_key_states[key];
    }
    [[nodiscard]] static InputController& get()
    {
        static InputController instance;
        return instance;
    }

private:
    InputController() = default;

    const uint8_t* m_key_states = nullptr;
};
}

#endif  // INPUT_CONTROLLER_H
