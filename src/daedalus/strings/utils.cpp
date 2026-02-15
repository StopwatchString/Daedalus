#include "daedalus/strings/utils.h"

#include <algorithm>
#include <cctype>

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#endif

namespace daedalus::strings
{
auto get_line(const char* str, size_t max_search_size, char delim) -> std::string_view
{
    const char* endl = static_cast<const char*>(std::memchr(str, delim, max_search_size));
    const size_t len = endl != nullptr ? static_cast<size_t>(endl - str) : max_search_size;
    return {str, len};
}

auto split(const char* buf, size_t size, char delim) -> std::vector<std::string_view> // NOLINT
{
    const char* front = buf;
    const char* back = buf + size; // NOLINT
    std::vector<std::string_view> svs;
    while (front < back)
    {
        std::string_view s = get_line(front, back - front, delim);
        front += s.size() + 1; // NOLINT
        svs.push_back(s);
    }
    return svs;
}

auto trim(std::string_view sv) -> std::string_view
{
    static const char* space_characters = " \t\n\v\f\r";
    size_t front = sv.find_first_not_of(space_characters);
    if (front == std::string_view::npos)
        return {};
    size_t back = sv.find_last_not_of(space_characters);
    return sv.substr(front, back - front + 1);
}

auto is_all_whitespace(const std::string_view sv) -> bool
{
    return std::ranges::all_of(sv, [](unsigned char c) -> int { return std::isspace(c); });
}

#if defined(_WIN32)

auto to_wide(std::string_view sv) -> std::wstring
{
    if (sv.empty())
    {
        return {};
    }

    // Check what size is needed first, then preallocate enough space
    int size = MultiByteToWideChar(CP_UTF8, 0, sv.data(), (int)sv.size(), nullptr, 0);
    std::wstring wide_string(size, 0);

    // The actual call
    MultiByteToWideChar(CP_UTF8, 0, sv.data(), (int)sv.size(), wide_string.data(), size);

    return wide_string;
}

auto from_wide(std::wstring_view sv) -> std::string
{
    if (sv.empty())
    {
        return {};
    }
    // Check what size is needed first, then preallocate enough space
    int size = WideCharToMultiByte(CP_UTF8, 0, sv.data(), (int)sv.size(), nullptr, 0, nullptr, nullptr);
    std::string narrow_string(size, 0);

    // The actual call
    WideCharToMultiByte(CP_UTF8, 0, sv.data(), (int)sv.size(), narrow_string.data(), size, nullptr, nullptr);
    return narrow_string;
}

auto get_line_wide(const wchar_t* str, size_t max_search_size, wchar_t delim) -> std::wstring_view
{
    const wchar_t* endl = static_cast<const wchar_t*>(std::wmemchr(str, delim, max_search_size));
    const size_t len = endl != nullptr ? static_cast<size_t>(endl - str) : max_search_size;
    return {str, len};
}

auto split_wide(const wchar_t* buf, size_t size, wchar_t delim) -> std::vector<std::wstring_view>
{
    const wchar_t* front = buf;
    const wchar_t* back = buf + size; // NOLINT
    std::vector<std::wstring_view> svs;
    while (front < back)
    {
        std::wstring_view s = get_line_wide(front, back - front, delim);
        front += s.size() + 1; // NOLINT
        svs.push_back(s);
    }
    return svs;
}

#endif

} // namespace daedalus::strings
