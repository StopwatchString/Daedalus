#ifndef DAEDALUS_CONTAINERS_STACK_ARRAY_H
#define DAEDALUS_CONTAINERS_STACK_ARRAY_H

#include "daedalus/attributes.h"
#include "daedalus/containers/array_interface.h"
#include "daedalus/containers/container_settings.h"

namespace daedalus::containers
{

template <typename T, size_t Capacity, ManagementMode MMode = ManagementMode::Managed>
class stack_array : public array_interface<stack_array<T, Capacity, MMode>, T, MMode>
{
    struct ManualTracking
    {
    };
    struct ManagedTracking
    {
        bool states[Capacity]{false}; // NOLINT
    };

    using TrackingData = std::conditional_t<MMode == ManagementMode::Managed, ManagedTracking, ManualTracking>;

  public:
    [[nodiscard]] auto data() -> std::byte*
    {
        return &buffer[0];
    };

    [[nodiscard]] auto size() const -> size_t
    {
        return Capacity;
    }

  private:
    [[nodiscard]] auto element_is_init(size_t index) -> bool
    {
        if constexpr (MMode == ManagementMode::Managed)
        {
            return tracking_data.states[index];
        }
        else
        {
            return false;
        }
    }

    auto set_element_init_state(size_t index, bool init) -> void
    {
        if constexpr (MMode == ManagementMode::Managed)
        {
            tracking_data.states[index] = init;
        }
    }

    [[nodiscard]] auto raw_ptr(size_t index) -> std::byte*
    {
        return &buffer[sizeof(T) * index];
    }
    [[nodiscard]] auto raw_ptr(size_t index) const -> const std::byte*
    {
        return &buffer[sizeof(T) * index];
    }

    alignas(T) std::byte buffer[sizeof(T) * Capacity]; // NOLINT
    NO_UNIQUE_ADDRESS TrackingData tracking_data{};

    friend class array_interface<stack_array<T, Capacity, MMode>, T, MMode>;
};

} // namespace daedalus::containers

#endif
