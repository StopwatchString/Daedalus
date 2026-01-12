#ifndef DAEDALUS_THREAD_UTILS_H
#define DAEDALUS_THREAD_UTILS_H

#include <span>
#include <string_view>
#include <thread>

namespace daedalus
{
namespace thread_utils
{
/**
 * @brief Attempts to set the logical processor affinity of the given thread.
 *
 * This is an abstraction over native APIs for Windows and Linux. If you don't have a std::thread and only have a
 * native handle, then instead copy the implementation of this function for the required platform.
 *
 * The term 'logical processor' is derived from the MSDN definition here:
 * https://learn.microsoft.com/en-us/windows/win32/procthread/processor-groups and refers to logical processors exposed
 * by the CPU. This refers primarily to cases like hyperthreading, where the Operating System exposes a single
 * hyperthreaded cpu core as two separate logical processors.
 *
 * Currently on Windows this function will allow setting logical processor affinities of id 64 or less. Windows uses a
 * concept of processor groups, each of which can contain only 64 options, and referring to logical processors beyond
 * the default processor group requires changing the processor group of the thread. This implementation ignores that
 * detail and just assumes the processor can have at most 64 logical processors.
 *
 * https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-setthreadaffinitymask
 *
 * @param thread The std::thread to attempt to set logical processor affinity on.
 * @param target_logical_processor_ids A 0-indexed span of logical processors to set affinity to. On Windows, expects a
 * given id to be in the range [0, 63].
 *
 * @return True if setting affinity was reported successful by the Operating System (this does not necessarily guarantee
 * expected behavior).
 */
bool set_thread_logical_processor_affinity(std::thread& thread, std::span<uint16_t> target_logical_processor_ids);

/**
 * @brief Attempts to set the string name associated with a thread at the operating system level.
 *
 * @param thread The std::thread to attempt to set the name of.
 * @param name The name to attempt to set on the thread.
 *
 * @return True if the thread's name was reported successfully set by the operating system.
 */
bool set_thread_name(std::thread& thread, std::string_view name);

} // namespace thread_utils
} // namespace daedalus

#endif
