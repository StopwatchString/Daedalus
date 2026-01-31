#include "daedalus/fileio.h"

#include <filesystem>
#include <new>
#include <utility>

namespace daedalus::fileio
{

auto free_file(File& file) -> void
{
    switch (file.allocation_type)
    {
    case AllocationType::Aligned:
        ::operator delete(file.buffer, std::align_val_t(file.alignment));
        break;
    case AllocationType::Unaligned:
        ::operator delete(file.buffer);
        break;
    default:
        std::unreachable();
    }
}

auto is_usable_directory_path(std::string_view directory_path) -> bool
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

} // namespace daedalus::fileio
