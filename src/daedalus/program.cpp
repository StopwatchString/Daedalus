#include "daedalus/program.h"

#include <span>

namespace daedalus::program
{
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

#ifdef DAEDALUS_MAIN

auto main(int argc, char** argv) -> int
{
    ProgramContext program_context{};

    program_context.args = daedalus::program::parse_args(argc, argv);
    program_context.environment = daedalus::program::get_environment();

    return daedalus_main(program_context);
}

#endif
