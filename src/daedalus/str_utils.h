#ifndef DAEDALUS_STR_UTILS_H
#define DAEDALUS_STR_UTILS_H

#include <string>
#include <string_view>
#include <vector>

namespace daedalus::str_utils
{

/**
 * @brief Given a character buffer, returns a string_view of the first line in the buffer delineated by a passed in
 * delimiter, up to a maximum search size.
 *
 * @note This function can be easily used to parse an entire buffer, by repeatedly calling it with the same buffer,
 * while incrementing the pointer to the buffer by the size of the last returned string_view.
 *
 * @param str The string buffer to search.
 * @param max_search_size The most characters that can be searched for the delimiter.
 * @param delim The delimiter to search for to delineate the string.
 *
 * @return A std::string_view of the first line encountered.
 */
[[nodiscard]] auto get_line_wide(const char* str, size_t max_search_size, char delim) -> std::string_view;

/**
 * @brief Splits a char buffer into a vector of string_views based on a given delimiter.
 *
 * @note The string_views are views of the passed in buffer, so the buffer must be kept alive while the string_views are
 * in use.
 *
 * @param buf A character buffer to split.
 * @param size The size of the character buffer.
 * @param delim The delimiter to split the character buffer with.
 *
 * @return A vector of string_views based on buf.
 */
[[nodiscard]] auto split(const char* buf, size_t size, char delim) -> std::vector<std::string_view>;

/**
 * @brief Removes the whitespace from the front and back of a string_view.
 *
 * @param sv The string view to trim whitespace from.
 *
 * @return A string_view of the passed in string without whitespace in the front or back.
 */
[[nodiscard]] auto trim(std::string_view sv) -> std::string_view;

/**
 * @brief Checks if a string is entirely whitespace.
 *
 * @param sv The string view to check for whitespace.
 *
 * @return True if the string is entirely whitespace, false otherwise.
 */
[[nodiscard]] auto is_all_whitespace(const std::string_view sv) -> bool;

#ifdef _WIN32
/**
 * @brief Converts a string to a Windows wide string.
 *
 * @note This function makes a copy of the string.
 *
 * @param sv The string view to convert.
 *
 * @return std::wstring containing the wide string.
 */
[[nodiscard]] auto to_wide(std::string_view sv) -> std::wstring;

/**
 * @brief Converts a Windows wide string to a string.
 *
 * @note This function makes a copy of the wide string.
 *
 * @param wsv The wide string view to convert.
 *
 * @return `std::string` containing the converted string.
 */
[[nodiscard]] auto from_wide(std::wstring_view wsv) -> std::string;

[[nodiscard]] auto get_line_wide(const wchar_t* str, size_t max_search_size, wchar_t delim) -> std::wstring_view;

[[nodiscard]] auto split_wide(const wchar_t* buf, size_t size, wchar_t delim) -> std::vector<std::wstring_view>;

#endif

} // namespace daedalus::str_utils

#endif
