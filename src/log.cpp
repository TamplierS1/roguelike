#include "fmt/core.h"

#include "log.h"

namespace Rg::Log
{
void error(std::string_view msg)
{
    fmt::print("Error: {}\n", msg.data());
}

void warning(std::string_view msg)
{
    fmt::print("Warning: {}\n", msg.data());
}

}
