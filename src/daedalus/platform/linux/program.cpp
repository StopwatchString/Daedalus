#include "daedalus/program/program.h"

#include <iostream>

namespace dae
{
auto get_environment() -> std::unordered_map<std::string, std::string>
{
    std::unordered_map<std::string, std::string> environment;

    std::cerr << "daedalus::program::get_environment() UNIMPLEMENTED ON LINUX \n";

    return environment;
}

auto get_executable_path() -> std::filesystem::path
{
    return {};
}

} // namespace dae
