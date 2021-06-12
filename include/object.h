#ifndef OBJECT_H
#define OBJECT_H

#include <unordered_map>

#include "vec2.h"
#include "sdl_wrappers/texture.h"
#include "actions/action.h"
#include "common.h"

namespace Rg
{
struct Camera;

class Object
{
public:
    Object(Vec2 pos, const s_ptr<Texture>& texture, const s_ptr<Renderer>& renderer)
        : m_pos(pos)
        , m_texture(texture)
        , m_renderer(renderer)
    {
    }

    void render(const Camera& camera, Color color = {255, 255, 255, 255});
    [[nodiscard]] Actions::IAction* get_action()
    {
        auto action = m_current_action;
        m_current_action.reset();
        return action.get();
    }
    constexpr void set_action(const s_ptr<Actions::IAction>& action)
    {
        m_current_action = action;
    }
    constexpr void regen_energy()
    {
        m_energy += m_energy_regen;
    }

    Vec2 m_pos;
    // Every action consumes energy.
    // Every object gets some amount of energy every turn.
    // Faster actors get more energy. 100 is base for everything.
    int m_energy = 100;

private:
    w_ptr<Texture> m_texture;
    w_ptr<Renderer> m_renderer;

    s_ptr<Actions::IAction> m_current_action;

    int m_energy_regen = 50;
};
}

#endif  // OBJECT_H
