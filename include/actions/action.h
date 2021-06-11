#ifndef ACTION_H
#define ACTION_H

namespace Rg
{
class Object;

namespace Actions
{
enum class Costs
{
    Move = 50,
};

class IAction
{
public:
    virtual ~IAction() = default;
    virtual bool execute(Object* object) = 0;
};
}
}

#endif  // ACTION_H
