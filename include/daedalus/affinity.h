#ifndef DAEDALUS_AFFINITY_H
#define DAEDALUS_AFFINITY_H

#include "daedalus/types.h"

#include <span>
#include <thread>

namespace daedalus
{
namespace affinity
{
/**
 * @brief Attempts to set the logical processor affinity of the given thread.
 *
 * This is an abstraction over native APIs for Windows and Linux. If you don't have a std::thread and only have a
 * native handle, then instead copy the implementation of this function for the required platform.
 *
 * The term 'logical processor' is derived from the MSDN definition
 * https://learn.microsoft.com/en-us/windows/win32/procthread/processor-groups
 * and refers not to physical CPU cores but to
 *
 * Currently on Windows this function will only work for logical processor counts of 64 or less. Windows uses a concept
 * of processor groups, each of which can contain only 64 options. This implementation ignores that detail and just
 * assumes the processor can have at most 64 logical processors.
 *
 * https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-setthreadaffinitymask
 *
 * @param thread The std::thread to attempt to set logical processor affinity on.
 * @param target_logical_processor_ids A 0-indexed span of logical processors to set affinity to.
 *
 * @return True if setting affinity was reported successful by the Operating System (this does not necessarily guarantee
 * expected behavior).
 */
bool set_thread_logical_processor_affinity(std::thread& thread,
                                           std::span<daedalus::primitives::u16> target_logical_processor_ids);
} // namespace affinity
} // namespace daedalus

#endif
