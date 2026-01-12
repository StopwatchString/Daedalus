#include "daedalus/str_utils.h"

#include <algorithm>
#include <cctype>

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#endif

namespace daedalus
{
namespace str_utils
{
std::string_view get_line(const char* str, size_t max_search_size, char delim)
{
    const char* endl = reinterpret_cast<const char*>(std::memchr(str, delim, max_search_size));
    if (endl == nullptr)
    {
        endl = str + max_search_size;
    }
    return std::string_view(str, endl - str);
}

std::vector<std::string_view> split(const char* buf, size_t size, char delim)
{
    const char* front = buf;
    const char* back = buf + size;
    std::vector<std::string_view> svs;
    while (front < back)
    {
        std::string_view s = get_line(front, back - front, delim);
        front += s.size() + 1;
        svs.push_back(s);
    }
    return svs;
}

std::string_view trim(std::string_view sv)
{
    static const char* space_characters = " \t\n\v\f\r";
    size_t front = sv.find_first_not_of(space_characters);
    if (front == std::string_view::npos)
        return {};
    size_t back = sv.find_last_not_of(space_characters);
    return sv.substr(front, back - front + 1);
}

bool is_all_whitespace(const std::string_view sv)
{
    return std::all_of(sv.begin(), sv.end(), [](unsigned char c) { return std::isspace(c); });
}

#if defined(_WIN32)

std::wstring to_wide(std::string_view sv)
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

#endif

} // namespace str_utils
} // namespace daedalus
