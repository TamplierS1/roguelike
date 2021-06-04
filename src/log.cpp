#include "fmt/core.h"

#include "log.h"

namespace Rg
{
namespace Log
{

void error(std::string_view msg)
{
    fmt::print("Error: {}\n", msg.data());
}

}
}
