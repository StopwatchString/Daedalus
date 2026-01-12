#include "daedalus/thread_utils.h"

#include "daedalus/str_utils.h"

namespace daedalus
{
namespace thread_utils
{

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

DWORD_PTR make_logical_processor_affinity_mask(std::span<uint16_t> target_logical_processor_ids)
{
    DWORD_PTR mask = 0;
    for (uint16_t id : target_logical_processor_ids)
    {
        mask |= static_cast<DWORD_PTR>(1) << id;
    }
    return mask;
}

bool set_thread_logical_processor_affinity(std::thread& thread, std::span<uint16_t> target_logical_processor_ids)
{
    DWORD_PTR target_mask = make_logical_processor_affinity_mask(target_logical_processor_ids);

    DWORD_PTR prev_mask = SetThreadAffinityMask(thread.native_handle(), target_mask);
    return prev_mask != 0;
}

bool set_thread_name(std::thread& thread, std::string_view name)
{
    HRESULT hResult = SetThreadDescription(thread.native_handle(), daedalus::str_utils::to_wide(name).c_str());
    return SUCCEEDED(hResult);
}

#elif define(__linux__)

// TODO:: LINUX SUPPORT

#endif

} // namespace thread_utils
} // namespace daedalus
