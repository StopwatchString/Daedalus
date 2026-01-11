#ifndef DAEDALUS_FILE_IO_H
#define DAEDALUS_FILE_IO_H

#include "types.h"

#include <memory>
#include <optional>
#include <string_view>

namespace daedalus
{
namespace fileio
{

/**
 * @brief A struct to hold the data of a file. Contains a char buffer and size.
 */
struct File
{
    std::unique_ptr<char[]> buf;
    daedalus::primitives::u64 size{0};
};

/**
 * @brief Attempts to load a file into a buffer.
 *
 * @note If the file does not exist, or any issues are encountered, the optional will be empty.
 *
 * @param file_path The path to the file.
 *
 * @return File struct on success, or std::nullopt on failure.
 */
[[nodiscard]] std::optional<File> load_file(std::string_view file_path);

/**
 * @brief Checks that a string represents a potentially real directory path, such that create_directories would succeed
 * if called on it.
 *
 * @param directory_path The input directory path to check.
 *
 * @return True if the directory path could be created, false otherwise.
 */
[[nodiscard]] bool is_usable_directory_path(std::string_view directory_path);
} // namespace fileio
} // namespace daedalus

#endif
