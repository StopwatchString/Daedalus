#ifndef DAEDALUS_PLATFORM_WINDOWS_SELECTORS_H
#define DAEDALUS_PLATFORM_WINDOWS_SELECTORS_H

#include <optional>
#include <string>

namespace dae::selector
{
/**
 * @brief Opens the Windows folder selection dialogue and returns a string representing the user's selection if no
 * errors occur.
 *
 * @return A string representing the user's selection if successfully retreived.
 */
auto open_windows_folder_dialogue() -> std::optional<std::string>;
} // namespace dae::selector

#endif
