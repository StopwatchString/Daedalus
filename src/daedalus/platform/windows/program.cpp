#include "daedalus/program/program.h"

#include "daedalus/strings/utils.h"

#include <array>
#include <cwchar>
#include <format>
#include <string>
#include <string_view>

namespace dae
{
auto get_environment() -> std::unordered_map<std::string, std::string>
{
    std::unordered_map<std::string, std::string> environment;

    LPWCH environment_strings_wide = GetEnvironmentStringsW();
    if (environment_strings_wide == NULL)
    {
        std::string error = get_formatted_last_windows_error();
        // Print?
        return environment;
    }

    // Returned Environment Strings block terminates with two consecutive L'\0's
    wchar_t* p = environment_strings_wide;
    while (*p != L'\0' || *(p + 1) != L'\0')
    {
        ++p;
    }
    size_t length = p - environment_strings_wide;

    std::vector<std::wstring_view> environment_strs = dae::strings::split_wide(environment_strings_wide, length, L'\0');

    for (std::wstring_view wstr : environment_strs)
    {
        if (wstr.length() > 1)
        {
            std::string str = dae::strings::from_wide(wstr);
            char* pos = reinterpret_cast<char*>(std::memchr(str.data() + 1, '=', str.length() - 1));
            if (pos != nullptr)
            {
                size_t pos_index = pos - str.data();
                environment.emplace(std::string_view{str.data(), pos_index},
                                    std::string_view{str.data() + pos_index + 1});
            }
        }
    }

    BOOL success = FreeEnvironmentStringsW(environment_strings_wide);
    if (success == FALSE)
    {
        std::string error = get_formatted_last_windows_error();
        // Print?
    }

    return environment;
}

auto get_executable_path() -> std::filesystem::path
{
    std::array<wchar_t, MAX_PATH> buffer;
    GetModuleFileNameW(nullptr, buffer.data(), MAX_PATH);
    return std::filesystem::path(buffer.data());
}

auto get_last_windows_error() -> WindowsError
{
    WindowsError windows_error;
    windows_error.code = GetLastError();

    LPSTR message_buffer = nullptr;

    DWORD format_message_flags =
        FORMAT_MESSAGE_FROM_SYSTEM |     // Get a message from an error code
        FORMAT_MESSAGE_ALLOCATE_BUFFER | // Allocate the message buffer for us (since we don't know what size to expect)
        FORMAT_MESSAGE_IGNORE_INSERTS;   // Defends variable argument insertion (%1, %2) which technically could be
                                         // present in messages.

    DWORD length = FormatMessageA(format_message_flags,                     // DWORD   dwFlags,
                                  NULL,                                     // LPCVOID lpSource,
                                  windows_error.code,                       // DWORD   dwMessageId,
                                  NULL,                                     // DWORD   dwLanguageId,
                                  reinterpret_cast<LPSTR>(&message_buffer), // LPSTR   lpBuffer
                                  0,                                        // DWORD   nSize,
                                  NULL                                      // va_list *Arguments
    );

    if (length > 0 && message_buffer != nullptr)
    {
        // Remove trailing returns from message.
        while (length > 0 && (message_buffer[length - 1] == '\r' || message_buffer[length - 1] == '\n')) // NOLINT
        {
            --length;
        }
        windows_error.message = std::string(message_buffer, length);
        LocalFree(message_buffer);
    }

    return windows_error;
}

auto get_formatted_last_windows_error() -> std::string
{
    WindowsError windows_error = get_last_windows_error();
    return std::format("{} (0x{:08X}) {}",
                       windows_error.code,
                       windows_error.code,
                       windows_error.message.value_or("Unknown Error"));
}

} // namespace dae
