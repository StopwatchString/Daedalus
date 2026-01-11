#include "daedalus/fileio.h"

#include <filesystem>
#include <fstream>

namespace daedalus
{
namespace fileio
{

std::optional<File> load_file(std::string_view file)
{
    // Open file
    std::ifstream f(std::string(file), std::ios::binary | std::ios::ate);
    if (!f.is_open())
        return std::nullopt;

    // File size
    std::streamsize size = f.tellg();
    if (size <= 0)
        return std::nullopt;

    f.seekg(0, std::ios::beg);

    std::unique_ptr<char[]> buf = std::make_unique<char[]>(size);

    if (!f.read(buf.get(), size))
    {
        return std::nullopt;
    }

    return File{.buf = std::move(buf), .size = static_cast<size_t>(size)};
}

bool is_usable_directory_path(std::string_view directory_path)
{
    if (directory_path.empty())
        return false;

    try
    {
        std::filesystem::path path(directory_path);

        if (std::filesystem::exists(path))
        {
            return std::filesystem::is_directory(path);
        }

        // Check that the nearest existing ancestor is a directory
        std::filesystem::path ancestor = path;
        while (!ancestor.empty() && !std::filesystem::exists(ancestor))
        {
            ancestor = ancestor.parent_path();
        }

        return ancestor.empty() || std::filesystem::is_directory(ancestor);
    }
    catch (const std::filesystem::filesystem_error&)
    {
        return false;
    }
}

} // namespace fileio
} // namespace daedalus
