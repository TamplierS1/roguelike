#ifndef OBJECT_H
#define OBJECT_H

#include <unordered_map>

#include "components/component.h"
#include "vec2.h"
#include "common.h"

namespace Rg
{
class Object
{
public:
    explicit Object(Vec2 pos)
        : m_pos(pos)
    {
    }

    void update();
    void add_component(const std::string& name,
                       const s_ptr<Components::IComponent>& component);
    [[nodiscard]] Components::IComponent* get_component(const std::string& name);
    void remove_component(const std::string& name);

    Vec2 m_pos;

private:
    std::unordered_map<std::string, s_ptr<Components::IComponent>> m_components;
};
}

#endif  // OBJECT_H
