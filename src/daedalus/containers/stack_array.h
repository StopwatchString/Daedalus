#ifndef DAEDALUS_CONTAINERS_STACK_ARRAY_H
#define DAEDALUS_CONTAINERS_STACK_ARRAY_H

#include "daedalus/attributes.h"
#include "daedalus/containers/array_interface.h"
#include "daedalus/containers/container_settings.h"

namespace daedalus::containers
{

template <typename T, size_t Capacity, ManagementMode MMode, auto... ValsForDefaultT>
    requires(Capacity > 0)
class stack_array_impl // NOLINT
    : public array_interface<stack_array_impl<T, Capacity, MMode, ValsForDefaultT...>, T, MMode, ValsForDefaultT...>
{
    struct ManualTracking
    {
    };
    struct ManagedTracking
    {
        bool states[Capacity]{}; // NOLINT
    };
    using TrackingData = std::conditional_t<MMode >= ManagementMode::Managed, ManagedTracking, ManualTracking>;

  public:
    stack_array_impl() = default;
    ~stack_array_impl()
    {
        if constexpr (MMode >= ManagementMode::Managed)
        {
            this->destroy_all_elements();
        }
    }

    stack_array_impl(const stack_array_impl& other)
    {
    }
    auto operator=(const stack_array_impl& other) -> stack_array_impl& = delete;
    stack_array_impl(stack_array_impl&& other) noexcept = delete;
    auto operator=(stack_array_impl&& other) noexcept -> stack_array_impl& = delete;

    /**
     * @brief Get a non-const byte pointer for the container's data.
     *
     * @return Pointer to the container's data.
     */
    [[nodiscard]] auto data() -> std::byte*
    {
        return &buffer[0];
    };

    /**
     * @brief Get a const byte pointer for the container's data.
     *
     * @return Const pointer to the container's data.
     */
    [[nodiscard]] auto data() const -> const std::byte*
    {
        return &buffer[0];
    };

    /**
     * @brief Get the size of the container.
     *
     * @return The size of the container.
     */
    [[nodiscard]] auto size() const -> size_t
    {
        return Capacity;
    }

    /**
     * @brief In `Managed` mode, this sets the tracked initialization value of every field to `false`, effectively
     * resetting the array back to a fresh state, without calling any destructors. No-op in `Unmanaged` mode.
     *
     * @note This function should be used to 'cancel' an implementation's destructor cleanup logic while in `Managed`
     * mode. Calling this removes the container's ability to know which elements need to be destructed.
     */
    auto forget_all_elements() -> void
    {
        if constexpr (MMode >= ManagementMode::Managed)
        {
            tracking_data = TrackingData();
        }
    }

  private:
    /**
     * @brief Check if an element of the container is intialized.
     *
     * @param index The index of the value whose initialization to check.
     *
     * return True if the element has been initialized, otherwise false.
     */
    [[nodiscard]] auto element_is_init(size_t index) const -> bool
    {
        if constexpr (MMode >= ManagementMode::Managed)
        {
            return tracking_data.states[index];
        }
        else
        {
            return false;
        }
    }

    /**
     * @brief Set the initialized state of an element for tracking purposes.
     *
     * @param index The index of the element to set init state on.
     * @param init The init state to set on the element.
     */
    auto set_element_init_state(size_t index, bool init) -> void
    {
        if constexpr (MMode >= ManagementMode::Managed)
        {
            tracking_data.states[index] = init;
        }
    }

    /**
     * @brief Get a raw byte pointer, unlaundered, at the address of the given element index.
     *
     * @param index The index to get a raw pointer at.
     *
     * @return A byte pointer to the container's allocated data, unlaundered.
     */
    [[nodiscard]] auto raw_ptr(size_t index) -> std::byte*
    {
        return &buffer[sizeof(T) * index];
    }

    /**
     * @brief Get a const raw byte pointer, unlaundered, at the address of the given element index.
     *
     * @param index The index to get the raw pointer at.
     *
     * @return A const byte pointer to the container's allocated data, unlaundered.
     */
    [[nodiscard]] auto raw_ptr(size_t index) const -> const std::byte*
    {
        return &buffer[sizeof(T) * index];
    }

    alignas(T) std::byte buffer[sizeof(T) * Capacity]; // NOLINT
    NO_UNIQUE_ADDRESS TrackingData tracking_data{};

    friend class array_interface<stack_array_impl<T, Capacity, MMode, ValsForDefaultT...>,
                                 T,
                                 MMode,
                                 ValsForDefaultT...>;
};

template <typename T, size_t Capacity, auto... ValsForDefaultT>
using unmanaged_stack_array = stack_array_impl<T, Capacity, ManagementMode::Unmanaged, ValsForDefaultT...>;

template <typename T, size_t Capacity, auto... ValsForDefaultT>
using managed_stack_array = stack_array_impl<T, Capacity, ManagementMode::Managed, ValsForDefaultT...>;

template <typename T, size_t Capacity, auto... ValsForDefaultT>
using safe_stack_array = stack_array_impl<T, Capacity, ManagementMode::ManagedWithBoundsChecks, ValsForDefaultT...>;

} // namespace daedalus::containers

#endif
