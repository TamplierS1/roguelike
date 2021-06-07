#include <cassert>

#include "object.h"
#include "log.h"

namespace Rg
{
void Object::update()
{
    for (auto& component : m_components)
    {
        component.second->update(this);
    }
}

void Object::add_component(const std::string& name,
                           const s_ptr<Components::IComponent>& component)
{
    if (m_components.find(name) != m_components.end())
    {
        Log::warning("Warning: component `" + name + "` already exists.\n");
    }

    m_components.emplace(name, component);
}

Components::IComponent* Object::get_component(const std::string& name)
{
    assert(m_components.find(name) != m_components.end() &&
           "Failed to find the"
           "requested component.");

    return m_components[name].get();
}

void Object::remove_component(const std::string& name)
{
    assert(m_components.find(name) != m_components.end() &&
           "Failed to find the"
           "requested component.");

    m_components.erase(name);
}

}
