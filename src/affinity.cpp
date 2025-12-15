#include "daedalus/affinity.h"

#include "daedalus/types.h"
using namespace daedalus::primitives;

namespace daedalus
{
namespace affinity
{

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

DWORD_PTR make_logical_processor_affinity_mask(std::span<u16> target_logical_processor_ids)
{
    DWORD_PTR mask = 0;
    for (u16 id : target_logical_processor_ids)
    {
        mask |= static_cast<DWORD_PTR>(1) << id;
    }
    return mask;
}

bool set_thread_logical_processor_affinity(std::thread& thread, std::span<u16> target_logical_processor_ids)
{
    DWORD_PTR target_mask = make_logical_processor_affinity_mask(target_logical_processor_ids);

    DWORD_PTR prev_mask = SetThreadAffinityMask(thread.native_handle(), target_mask);
    return prev_mask != 0;
}

#elif define(__linux__)

// TODO:: LINUX SUPPORT

#endif

} // namespace affinity
} // namespace daedalus
