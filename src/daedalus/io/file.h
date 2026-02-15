#ifndef DAEDALUS_IO_FILE_H
#define DAEDALUS_IO_FILE_H

#include <functional>
#include <optional>
#include <string_view>

namespace daedalus::io
{

/**
 * @brief Enum to encode a strategy to use to load a file.
 */
enum class FileLoadStrategy : uint8_t
{
    Unset = 0,
    /**
     * @brief Use basic standard library implementations for fileio
     */
    StdLibrary,
    /**
     * @brief Allow the file load to route through a filesystem's virtual file caching if available.
     *
     * @note This is not always faster. Windows' file caching mechanism inflicts significant extra overhead compared to
     * a direct file read if the file is not prepared in the filesystem cache.
     */
    AllowCached,
    /**
     * @brief Force the file load to route directly from the disk if possible. 'Safe' indicates that this file load will
     * not attempt to lean on any special conventions exposed by the operating system, such as Windows' DirectStorage or
     * DAX (DirectAccess) features.
     */
    SafeDirectDisk,
};

/**
 * @brief Enum to track the type of allocation used for a daedalus::fileio::File's buffer allocation, to indicate the
 * strategy of deallocation.
 */
enum class AllocationType : uint8_t
{
    Unset = 0,
    Unaligned,
    Aligned
};

/**
 * @brief Useful metadata for a file on disk.
 */
struct FileMetaData
{
    uint64_t size{0};
    uint64_t alignment{0};
};

/**
 * @brief A struct to hold the data of a file.
 *
 * @note Must be manually freed with `daedalus::fileio::free_file()`, which will account for alignment properties of the
 * allocated buffer.
 */
struct File
{
    void* buffer{nullptr};
    uint64_t buffer_size{0};
    uint64_t bytes_read{0};
    uint64_t alignment{0};
    AllocationType allocation_type{};
};

using FileFuture = std::move_only_function<std::optional<File>()>;

/**
 * @brief Attempts to load a file into a buffer.
 *
 * @note If the file does not exist, or any issues are encountered, the optional will be empty.
 *
 * @note If a `daedalus::fileio::File` is successfully retreived with this function, it *must* be freed using
 * `daedalus::fileio::free_file()`
 *
 * @param file_path The path to the file.
 * @param load_strategy Guidance on the strategy to use to load the file. Can have significant impact on file loading
 * speed depending on usage.
 *
 * @return File struct on success, or std::nullopt on failure.
 */
[[nodiscard]] auto load_file(std::string_view file_path, FileLoadStrategy load_strategy = FileLoadStrategy::StdLibrary)
    -> std::optional<File>;

/**
 * @brief Kicks off an async task to load a file into a local buffer.
 *
 * @note If the file is not done being loaded when the callable is invoked, then the callable will block. This API does
 * not give access to checking on the status of the file read.
 *
 * @note If a `daedalus::fileio::File` is successfully retreived with this function, it *must* be freed using
 * `daedalus::fileio::free_file()`
 *
 * @param file_path The path to the file.
 * @param load_strategy Guidance on the strategy to use to load the file. Can have significant impact on file loading
 * speed depending on usage.
 *
 * @return If the file can be loaded asynchronously, will return a callable that when invoked will return the File or
 * block until the File is ready. On any failure, a nullopt will substitute the expected return.
 */
[[nodiscard]] auto load_file_async(std::string_view file_path,
                                   FileLoadStrategy load_strategy = FileLoadStrategy::StdLibrary)
    -> std::optional<FileFuture>;

/**
 * @brief Frees a file.
 *
 * @param file The file whose memory should be freed.
 */
auto free_file(File& file) -> void;

/**
 * @brief Tries to fill a FileMetaData struct using operating system APIs for a given file.
 *
 * @param file_path The path to the file.
 *
 * @return A complete FileMetaData struct if it can gather all information, std::nullopt otherwise.
 */
[[nodiscard]] auto get_file_meta_data(std::string_view file_path) -> std::optional<FileMetaData>;

/**
 * @brief Checks that a string represents a potentially real directory path, such that create_directories would succeed
 * if called on it.
 *
 * @param directory_path The input directory path to check.
 *
 * @return True if the directory path could be created, false otherwise.
 */
[[nodiscard]] auto is_usable_directory_path(std::string_view directory_path) -> bool;

} // namespace daedalus::io

#endif
