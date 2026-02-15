#ifndef DAEDALUS_PLATFORM_WINDOWS_TERMINAL_H
#define DAEDALUS_PLATFORM_WINDOWS_TERMINAL_H

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

namespace dae::windows::terminal
{
auto set_console_virtual_terminal_processing_enabled(bool enabled) -> bool;

auto set_console_eol_wrapping_enabled(bool enabled) -> bool;

auto set_console_disable_newline_auto_return_enabled(bool enabled) -> bool;

auto get_console_columns() -> size_t;

auto clear_console_buffer(HANDLE hConsole) -> bool;

} // namespace dae::windows::terminal

#endif
