#include "daedalus/io/file.h"

#include "daedalus/math/math.h"

#include <memory>
#include <new>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include <fstream>
#include <functional>

// NOLINTBEGIN(modernize-use-nullptr,cppcoreguidelines-pro-type-reinterpret-cast)

namespace daedalus::io
{

namespace
{
/**
 * @brief Uses the Windows API to query a file HANDLE for a file's logical disk alignment, to be used for allocating
 * user-buffers for uncached IO access.
 *
 * @param file The path to the file to open.
 *
 * @return A size_t of the file's alignment if it can be retreived.
 */
auto get_alignment_from_file(HANDLE hFile) -> std::optional<size_t>
{
    if (hFile == INVALID_HANDLE_VALUE)
    {
        return std::nullopt;
    }

    FILE_STORAGE_INFO file_storage_info{};

    BOOL result = GetFileInformationByHandleEx(hFile,
                                               FILE_INFO_BY_HANDLE_CLASS::FileStorageInfo,
                                               &file_storage_info,
                                               sizeof(FILE_STORAGE_INFO));
    if (result == FALSE)
    {
        return std::nullopt;
    }

    return file_storage_info.PhysicalBytesPerSectorForPerformance;
}

/**
 * @brief Uses the Windows API to query a file HANDLE for a file's size on disk to load.
 *
 * @param file The path to the file to open.
 *
 * @return A size_t of the file's size if it can be retreived.
 */
auto get_size_from_file(HANDLE hFile) -> std::optional<size_t>
{
    if (hFile == INVALID_HANDLE_VALUE)
    {
        return std::nullopt;
    }

    FILE_STANDARD_INFO file_standard_info{};

    BOOL result = GetFileInformationByHandleEx(hFile,
                                               FILE_INFO_BY_HANDLE_CLASS::FileStandardInfo,
                                               &file_standard_info,
                                               sizeof(FILE_STANDARD_INFO));
    if (result == FALSE)
    {
        DWORD error = GetLastError();
        return std::nullopt;
    }

    return file_standard_info.AllocationSize.QuadPart;
}

/**
 * @brief An implementation of load_file for FileLoadStrategy::StdLibrary
 *
 * @param file The path to the file.
 *
 * @return A File struct if successful.
 */
auto load_file_standard_library(std::string_view file) -> std::optional<File>
{
    // Open file
    std::ifstream f(std::string(file), std::ios::binary | std::ios::ate);
    if (!f.is_open())
        return std::nullopt;

    // File size
    std::streamsize size = f.tellg();
    if (size <= 0)
        return std::nullopt;

    void* buffer = ::operator new(size);

    f.seekg(0, std::ios::beg);
    if (!f.read(static_cast<char*>(buffer), size))
    {
        return std::nullopt;
    }

    return File{
        .buffer = buffer,
        .buffer_size = static_cast<size_t>(size),
        .bytes_read = static_cast<size_t>(size),
        .alignment = 1,
        .allocation_type = AllocationType::Unaligned,
    };
}

/**
 * @brief An implementation of load_file_async for FileLoadStrategy::StdLibrary
 *
 * @param file The path to the file.
 *
 * @return A callable that will optionally return a File struct if it succeeds.
 */
auto load_file_standard_library_async(std::string_view file)
    -> std::optional<std::move_only_function<std::optional<File>()>>
{
    return [file = std::string(file)]() -> std::optional<File> { return load_file_standard_library(file); };
}

/**
 * @brief An implementation of load_file for FileLoadStrategy::AllowCached
 *
 * @param file The path to the file.
 *
 * @return A File struct if successful.
 */
auto load_file_allow_cached(std::string_view file) -> std::optional<File>
{
    HANDLE hFile = CreateFileA(file.data(), // NOLINT(bugprone-suspicious-stringview-data-usage)
                               GENERIC_READ,
                               FILE_SHARE_READ,
                               NULL,
                               OPEN_EXISTING,
                               FILE_ATTRIBUTE_NORMAL,
                               NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        return std::nullopt;
    }

    std::optional<size_t> maybe_file_size = get_size_from_file(hFile);
    if (!maybe_file_size)
    {
        CloseHandle(hFile);
        return std::nullopt;
    }
    size_t file_size = maybe_file_size.value();

    File f{
        .buffer = ::operator new(file_size),
        .buffer_size = file_size,
        .bytes_read = 0,
        .alignment = 1,
        .allocation_type = AllocationType::Unaligned,
    };

    DWORD bytes_read = 0;
    BOOL result = ReadFile(hFile, f.buffer, static_cast<DWORD>(file_size), &bytes_read, NULL); // NOLINT

    f.bytes_read = bytes_read;

    CloseHandle(hFile);

    if (result == FALSE)
    {
        free_file(f);
        return std::nullopt;
    }

    return f;
}

/**
 * @brief An implementation of load_file_async for FileLoadStrategy::AllowCached
 *
 * @param file The path to the file.
 *
 * @return A callable that will optionally return a File struct if it succeeds.
 */
auto load_file_allow_cached_async(std::string_view file)
    -> std::optional<std::move_only_function<std::optional<File>()>>
{
    HANDLE hFile = CreateFileA(file.data(), // NOLINT(bugprone-suspicious-stringview-data-usage)
                               GENERIC_READ,
                               FILE_SHARE_READ,
                               NULL,
                               OPEN_EXISTING,
                               FILE_FLAG_OVERLAPPED,
                               NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        return std::nullopt;
    }

    std::optional<size_t> maybe_file_size = get_size_from_file(hFile);
    if (!maybe_file_size)
    {
        CloseHandle(hFile);
        return std::nullopt;
    }
    size_t file_size = maybe_file_size.value();

    File f{
        .buffer = ::operator new(file_size),
        .buffer_size = file_size,
        .alignment = 1,
        .allocation_type = AllocationType::Unaligned,
    };

    std::unique_ptr<OVERLAPPED> overlapped = std::make_unique<OVERLAPPED>();
    overlapped->hEvent = CreateEvent(NULL,  // Security Attributes
                                     TRUE,  // Manual Reset required
                                     FALSE, // Start signaled
                                     NULL); // Name

    BOOL result = ReadFile(hFile, f.buffer, static_cast<DWORD>(file_size), NULL, overlapped.get()); // NOLINT

    if (result == FALSE && GetLastError() != ERROR_IO_PENDING)
    {
        CloseHandle(overlapped->hEvent);
        CloseHandle(hFile);
        free_file(f);
        return std::nullopt;
    }

    return [overlapped = std::move(overlapped), hFile, f, file_size]() mutable -> std::optional<File> {
        DWORD bytes_read = 0;
        BOOL success = GetOverlappedResult(hFile, overlapped.get(), &bytes_read, TRUE);

        CloseHandle(overlapped->hEvent);
        CloseHandle(hFile);

        if (success)
        {
            f.bytes_read = bytes_read;
            return f;
        }
        else
        {
            free_file(f);
            return std::nullopt;
        }
    };
}

/**
 * @brief An implementation of load_file for FileLoadStrategy::SafeDirectDisk
 *
 * @param file The path to the file.
 *
 * @return A File struct if successful.
 */
auto load_file_safe_direct_disk(std::string_view file) -> std::optional<File>
{
    HANDLE hFile = CreateFileA(file.data(), // NOLINT(bugprone-suspicious-stringview-data-usage)
                               GENERIC_READ,
                               FILE_SHARE_READ,
                               NULL,
                               OPEN_EXISTING,
                               FILE_FLAG_NO_BUFFERING,
                               NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        return std::nullopt;
    }

    std::optional<size_t> maybe_file_size = get_size_from_file(hFile);
    std::optional<size_t> maybe_file_alignment = get_size_from_file(hFile);
    if (!maybe_file_size || !maybe_file_alignment)
    {
        CloseHandle(hFile);
        return std::nullopt;
    }
    size_t file_size = maybe_file_size.value();
    size_t file_alignment = maybe_file_alignment.value();

    // When NO_BUFFERING is specified, file size to work with has to be a multiple of the alignment.
    file_size = daedalus::math::align_up(file_size, file_alignment);

    File f{
        .buffer = ::operator new(file_size, std::align_val_t(file_alignment)),
        .buffer_size = file_size,
        .alignment = file_alignment,
        .allocation_type = AllocationType::Aligned,
    };

    DWORD bytes_read = 0;
    BOOL result = ReadFile(hFile, f.buffer, static_cast<DWORD>(file_size), &bytes_read, NULL); // NOLINT

    f.bytes_read = bytes_read;

    CloseHandle(hFile);

    if (result == FALSE)
    {
        free_file(f);
        return std::nullopt;
    }

    return f;
}

/**
 * @brief An implementation of load_file_async for FileLoadStrategy::SafeDirectDisk
 *
 * @param file The path to the file.
 *
 * @return A callable that will optionally return a File struct if it succeeds.
 */
auto load_file_safe_direct_disk_async(std::string_view file)
    -> std::optional<std::move_only_function<std::optional<File>()>>
{
    HANDLE hFile = CreateFileA(file.data(), // NOLINT(bugprone-suspicious-stringview-data-usage)
                               GENERIC_READ,
                               FILE_SHARE_READ,
                               NULL,
                               OPEN_EXISTING,
                               FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING,
                               NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        return std::nullopt;
    }

    std::optional<size_t> maybe_file_size = get_size_from_file(hFile);
    std::optional<size_t> maybe_file_alignment = get_size_from_file(hFile);
    if (!maybe_file_size || !maybe_file_alignment)
    {
        CloseHandle(hFile);
        return std::nullopt;
    }
    size_t file_size = maybe_file_size.value();
    size_t file_alignment = maybe_file_alignment.value();

    // When NO_BUFFERING is specified, file size to work with has to be a multiple of the alignment.
    file_size = daedalus::math::align_up(file_size, file_alignment);

    File f{
        .buffer = ::operator new(file_size, std::align_val_t(file_alignment)),
        .buffer_size = file_size,
        .alignment = file_alignment,
        .allocation_type = AllocationType::Aligned,
    };

    std::unique_ptr<OVERLAPPED> overlapped = std::make_unique<OVERLAPPED>();
    overlapped->hEvent = CreateEvent(NULL,  // Security Attributes
                                     TRUE,  // Manual Reset required
                                     FALSE, // Start signaled
                                     NULL); // Name

    BOOL result = ReadFile(hFile, f.buffer, static_cast<DWORD>(file_size), NULL, overlapped.get()); // NOLINT

    if (result == FALSE && GetLastError() != ERROR_IO_PENDING)
    {
        CloseHandle(overlapped->hEvent);
        CloseHandle(hFile);
        free_file(f);
        return std::nullopt;
    }

    return [overlapped = std::move(overlapped), hFile, f, file_size]() mutable -> std::optional<File> {
        DWORD bytes_read = 0;
        BOOL success = GetOverlappedResult(hFile, overlapped.get(), &bytes_read, TRUE);

        CloseHandle(overlapped->hEvent);
        CloseHandle(hFile);

        if (success)
        {
            f.bytes_read = bytes_read;
            return f;
        }
        else
        {
            free_file(f);
            return std::nullopt;
        }
    };
}

} // namespace

auto load_file(std::string_view file, FileLoadStrategy load_strategy) -> std::optional<File>
{
    switch (load_strategy)
    {
    case FileLoadStrategy::StdLibrary:
        return load_file_standard_library(file);
    case FileLoadStrategy::AllowCached:
        return load_file_allow_cached(file);
    case FileLoadStrategy::SafeDirectDisk:
        return load_file_safe_direct_disk(file);
    default:
        return std::nullopt;
    }
}

auto load_file_async(std::string_view file, FileLoadStrategy load_strategy)
    -> std::optional<std::move_only_function<std::optional<File>()>>
{
    switch (load_strategy)
    {
    case FileLoadStrategy::StdLibrary:
        return load_file_standard_library_async(file);
    case FileLoadStrategy::AllowCached:
        return load_file_allow_cached_async(file);
    case FileLoadStrategy::SafeDirectDisk:
        return load_file_safe_direct_disk_async(file);
    default:
        return std::nullopt;
    }
}

auto get_file_meta_data(std::string_view file) -> std::optional<FileMetaData>
{
    HANDLE hFile = CreateFileA(file.data(), // NOLINT(bugprone-suspicious-stringview-data-usage)
                               GENERIC_READ,
                               FILE_SHARE_READ,
                               NULL,
                               OPEN_EXISTING,
                               FILE_ATTRIBUTE_NORMAL,
                               NULL);
    if (!SUCCEEDED(hFile))
    {
        return std::nullopt;
    }

    std::optional<size_t> maybe_size = get_size_from_file(hFile);
    std::optional<size_t> maybe_alignment = get_alignment_from_file(hFile);

    CloseHandle(hFile);

    if (!maybe_alignment || !maybe_size)
    {
        return std::nullopt;
    }

    return FileMetaData{
        .size = maybe_size.value(),
        .alignment = maybe_alignment.value(),
    };
}

} // namespace daedalus::io

// NOLINTEND(modernize-use-nullptr,cppcoreguidelines-pro-type-reinterpret-cast)
