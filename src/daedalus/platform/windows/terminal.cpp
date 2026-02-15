#include "daedalus/platform/windows/terminal.h"

namespace dae::windows::terminal
{
auto set_console_virtual_terminal_processing_enabled(bool enabled) -> bool
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE)
        return false;

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode))
        return false;

    // Enable the virtual terminal processing flag
    if (enabled)
    {
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    }
    else
    {
        dwMode &= ~ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    }
    return SetConsoleMode(hOut, dwMode);
}

auto set_console_eol_wrapping_enabled(bool enabled) -> bool
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE)
        return false;

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode))
        return false;

    if (enabled)
    {
        dwMode |= ENABLE_WRAP_AT_EOL_OUTPUT;
    }
    else
    {
        dwMode &= ~ENABLE_WRAP_AT_EOL_OUTPUT;
    }
    return SetConsoleMode(hOut, dwMode);
}

auto set_console_disable_newline_auto_return_enabled(bool enabled) -> bool
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE)
        return false;

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode))
        return false;

    if (enabled)
    {
        dwMode |= DISABLE_NEWLINE_AUTO_RETURN;
    }
    else
    {
        dwMode &= ~DISABLE_NEWLINE_AUTO_RETURN;
    }
    return SetConsoleMode(hOut, dwMode);
}

auto get_console_columns() -> size_t
{
    auto width_from = [](HANDLE h) -> int {
        CONSOLE_SCREEN_BUFFER_INFO csbi{};
        if (GetConsoleScreenBufferInfo(h, &csbi))
        {
            return static_cast<int>(csbi.srWindow.Right - csbi.srWindow.Left + 1);
        }
        return 0;
    };

    // Try STDOUT first (may be a pipe)
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE)
    {
        if (DWORD type = GetFileType(hOut); type == FILE_TYPE_CHAR)
        {
            if (int w = width_from(hOut))
                return w;
        }
    }

    // Fallback to the active screen buffer even if stdout is redirected
    HANDLE hConOut = CreateFileW(L"CONOUT$",
                                 GENERIC_READ | GENERIC_WRITE,
                                 FILE_SHARE_READ | FILE_SHARE_WRITE,
                                 nullptr,
                                 OPEN_EXISTING,
                                 0,
                                 nullptr);
    if (hConOut != INVALID_HANDLE_VALUE)
    {
        int w = width_from(hConOut);
        CloseHandle(hConOut);
        return w;
    }
    return 0; // no console
}

auto clear_console_buffer(HANDLE hConsole) -> bool
{
    if (hConsole == INVALID_HANDLE_VALUE)
        return false;

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
        return false;

    DWORD consoleSize = csbi.dwSize.X * csbi.dwSize.Y;
    COORD homeCoord = {0, 0};
    DWORD charsWritten{};

    // Fill with spaces
    if (!FillConsoleOutputCharacter(hConsole, TEXT(' '), consoleSize, homeCoord, &charsWritten))
        return false;

    // Reset attributes
    if (!FillConsoleOutputAttribute(hConsole, csbi.wAttributes, consoleSize, homeCoord, &charsWritten))
        return false;

    // Move cursor back to top-left
    if (!SetConsoleCursorPosition(hConsole, homeCoord))
        return false;

    return true;
}

} // namespace dae::windows::terminal
