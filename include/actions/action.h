#include <utility>
#include <string>

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
    virtual void execute(Object* object) = 0;
};

enum class FailReason
{
    SpaceIsOccupied,
    NotEnoughEnergy
};

class ActionFailed : public std::exception
{
public:
    ActionFailed(FailReason why_failed, std::string msg)
        : m_failure_reason(why_failed)
        , m_msg(std::move(msg))
    {
    }

    [[nodiscard]] const char* what() const noexcept override
    {
        return m_msg.c_str();
    }

    FailReason m_failure_reason;
    std::string m_msg;
};
}

}

#endif  // ACTION_H
