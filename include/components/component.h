#ifndef COMPONENT_H
#define COMPONENT_H

namespace Rg
{
class Object;

namespace Components
{
class IComponent
{
public:
    virtual ~IComponent() = default;
    virtual void update(Object* object) = 0;
};
}

}

#endif  // COMPONENT_H
