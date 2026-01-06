#ifndef DAEDALUS_STR_UTILS_H
#define DAEDALUS_STR_UTILS_H

#include <string_view>
#include <vector>

namespace daedalus
{
namespace str_utils
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
[[nodiscard]] std::string_view get_line(const char* str, size_t max_search_size, char delim);

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
[[nodiscard]] std::vector<std::string_view> split(const char* buf, size_t size, char delim);

/**
 * @brief Removes the whitespace from the front and back of a string_view.
 *
 * @param sv The string view to trim whitespace from.
 *
 * @return A string_view of the passed in string without whitespace in the front or back.
 */
[[nodiscard]] std::string_view trim(std::string_view sv);

/**
 * @brief Checks if a string is entirely whitespace.
 *
 * @param sv The string view to check for whitespace.
 *
 * @return True if the string is entirely whitespace, false otherwise.
 */
[[nodiscard]] bool is_all_whitespace(const std::string_view sv);

} // namespace str_utils
} // namespace daedalus

#endif
