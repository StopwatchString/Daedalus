#include "daedalus/fileio.h"

#include <iostream>

namespace daedalus::fileio
{
auto load_file(std::string_view file, FileLoadStrategy load_strategy) -> std::optional<File>
{
    std::cerr << "daedalus::fileio::load_file() UNIMPLEMENTED ON LINUX \n";
    return std::nullopt;
}

auto load_file_async(std::string_view file, FileLoadStrategy load_strategy)
    -> std::optional<std::move_only_function<std::optional<File>()>>
{
    std::cerr << "daedalus::fileio::load_file_async() UNIMPLEMENTED ON LINUX \n";
    return std::nullopt;
}

auto get_file_meta_data(std::string_view file) -> std::optional<FileMetaData>
{
    std::cerr << "daedalus::fileio::get_file_meta_data() UNIMPLEMENTED ON LINUX \n";
    return std::nullopt;
}
} // namespace daedalus::fileio
