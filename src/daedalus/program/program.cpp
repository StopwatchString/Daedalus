#include "daedalus/program/program.h"

#include <span>

namespace daedalus::program
{
auto get_program_meta(int argc, char** argv) -> ProgramMeta
{
    ProgramMeta meta{};

    meta.args = parse_args(argc, argv);
    meta.environment = get_environment();
    meta.working_directory = std::filesystem::current_path();
    meta.executable_absolute_path = get_executable_path();

    return meta;
}

auto parse_args(int argc, char** argv) -> std::vector<std::string_view>
{
    std::span<char*> args_span{argv, static_cast<std::size_t>(argc)};
    std::vector<std::string_view> args_vector;
    args_vector.reserve(argc);
    for (size_t i = 0; i < argc; i++)
    {
        args_vector.emplace_back(args_span[i]);
    }
    return args_vector;
}
} // namespace daedalus::program
