#include "daedalus/program.h"

#include <iostream>

namespace daedalus::program
{
auto get_environment() -> std::unordered_map<std::string, std::string>
{
    std::unordered_map<std::string, std::string> environment;

    std::cerr << "daedalus::program::get_environment() UNIMPLEMENTED ON LINUX \n";

    return environment;
}
} // namespace daedalus::program
