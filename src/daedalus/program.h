#ifndef DAEDALUS_PROGRAM_H
#define DAEDALUS_PROGRAM_H

#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#endif

namespace daedalus::program
{
/**
 * @brief Retrieves a map of all environment variables in the program.
 *
 * @note Uses platform-specific logic.
 *
 * @return A hashmap of the current environment variables.
 */
[[nodiscard]] auto get_environment() -> std::unordered_map<std::string, std::string>;

/**
 * @brief Convenience method for parsing typical `main()` args into
 * a more convenient structure.
 *
 * @param argc The argc passed to `main()`
 * @param argv The argv passed to `main()`
 *
 * @return A vector of string_views parsed from the arg list.
 */
[[nodiscard]] auto parse_args(int argc, char** argv) -> std::vector<std::string_view>;

#ifdef _WIN32

struct WindowsError
{
    DWORD code{};
    std::optional<std::string> message;
};

/**
 * @brief Shorthand for calling `GetLastError()` followed by passing the error
 * to `FormatMessage()`.
 *
 * @note If Windows fails to look up the associated string with the error code, then the string will be an empty
 * optional. This likely means the error was not a system error but something manually set by your code or a third party
 * library. Additionally, when the failure occurs, this function will set the most recent error code.
 *
 * @return A `WindowsError` struct that contains the `DWORD` returned by `GetLastError` and possibly
 * a string that the error maps to if Windows can find it.
 */
[[nodiscard]] auto get_last_windows_error() -> WindowsError;

/**
 * @brief Shorthand for calling `get_last_windows_error()` and formatting the `WindowsError` into a string as
 * `error_code_dec (error_code_hex) error_message`.
 *
 * @note If `error_message` cannot be retrieved, then placeholder text is used instead of failure.
 *
 * @return A formatted string of the last Windows error.
 */
[[nodiscard]] auto get_formatted_last_windows_error() -> std::string;

#endif

} // namespace daedalus::program

#define DAEDALUS_MAIN
#ifdef DAEDALUS_MAIN

struct ProgramContext
{
    std::vector<std::string_view> args;
    std::unordered_map<std::string, std::string> environment;
};

auto daedalus_main(ProgramContext program_context) -> int;

#endif

#endif
