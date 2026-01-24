#ifndef DAEDALUS_CONTAINERS_ARRAY_INTERFACE_H
#define DAEDALUS_CONTAINERS_ARRAY_INTERFACE_H

#include "daedalus/containers/container_settings.h"

#include <cstddef>
#include <iterator>
#include <memory>
#include <utility>

namespace daedalus::containers
{

/**
 * @brief An array interface that is lightweight and prioritizes only doing things explicitly asked of it. Implements
 * most functionality itself, and exposes just a few required specialization functions via CRTP to implementations.
 *
 * @param Derived The CRTP injection parameter
 * @param T The type the container holds
 * @param MMode The ManagementMode of the container. In Unmanaged mode, the container will never manually call a
 * destructor for any element. In Managed mode, the container will call destructors for elements when they are overriden
 * or when the array is destroyed.
 */
template <typename Derived, typename T, ManagementMode MMode>
class array_interface
{
    /**
     * Implementer's CRTP Interface
     */
  public:
    [[nodiscard]] auto data() -> std::byte*
    {
        return static_cast<Derived*>(this)->data();
    };

    [[nodiscard]] auto data() const -> const std::byte*
    {
        return static_cast<const Derived*>(this)->data();
    };

    [[nodiscard]] auto size() const -> size_t
    {
        return static_cast<const Derived*>(this)->size();
    }

  private:
    [[nodiscard]] auto element_is_init(size_t index) const -> bool
    {
        return static_cast<const Derived*>(this)->element_is_init(index);
    }

    auto set_element_init_state(size_t index, bool init) -> void
    {
        static_cast<Derived*>(this)->set_element_init_state(index, init);
    }

    [[nodiscard]] auto raw_ptr(size_t index) -> std::byte*
    {
        return static_cast<Derived*>(this)->raw_ptr(index);
    }
    [[nodiscard]] auto raw_ptr(size_t index) const -> const std::byte*
    {
        return static_cast<const Derived*>(this)->raw_ptr(index);
    }

    /**
     * Implemented Interface
     */
  public:
    auto copy_to(size_t index, const T& value) -> void
    {
        if constexpr (MMode == ManagementMode::Managed)
        {
            if (element_is_init(index))
            {
                std::destroy_at(laundered_element_ptr(index));
            }
        }
        std::construct_at(element_ptr(index), value);
        if constexpr (MMode == ManagementMode::Managed)
        {
            set_element_init_state(index, true);
        }
    }

    auto move_to(size_t index, T&& value) -> void
    {
        if constexpr (MMode == ManagementMode::Managed)
        {
            if (element_is_init(index))
            {
                std::destroy_at(laundered_element_ptr(index));
            }
        }
        std::construct_at(element_ptr(index), std::move(value));
        if constexpr (MMode == ManagementMode::Managed)
        {
            set_element_init_state(index, true);
        }
    }

    template <typename... Args>
    auto create_at(size_t index, Args&&... args) -> void
    {
        if constexpr (MMode == ManagementMode::Managed)
        {
            if (element_is_init(index))
            {
                std::destroy_at(laundered_element_ptr(index));
            }
        }
        std::construct_at(element_ptr(index), std::forward<Args>(args)...);
        if constexpr (MMode == ManagementMode::Managed)
        {
            set_element_init_state(index, true);
        }
    }

    auto destroy_at(size_t index) -> void
    {
        if constexpr (MMode == ManagementMode::Managed)
        {
            if (element_is_init(index))
            {
                std::destroy_at(laundered_element_ptr(index));
            }
            set_element_init_state(index, false);
        }
        else
        {
            std::destroy_at(laundered_element_ptr(index));
        }
    }

    auto copy_from(size_t index) const -> T
    {
        return *laundered_element_ptr(index);
    }

    auto move_from(size_t index) -> T&&
    {
        if constexpr (MMode == ManagementMode::Managed)
        {
            set_element_init_state(index, false);
        }
        return std::move(*laundered_element_ptr(index));
    }

    auto ref_from(size_t index) -> T&
    {
        return *laundered_element_ptr(index);
    }

    auto const_ref_from(size_t index) const -> const T&
    {
        return *laundered_element_ptr(index);
    }

    /**
     * @brief Calls std::destroy_at() on all elements of the array. WARNING: In Unmanaged mode, this is done
     * indiscriminately and if the contained type has a nontrivial destructor, you're liable to blow something up if the
     * whole array isn't initialized. In Managed mode, std::destroy_at() is only called on elements marked as
     * initialized.
     *
     * @note After this function is called, the entire array is assumed to be uninitialized.
     *
     * @note Derived classes should use this to implement their destructor in Managed mode.
     */
    auto destroy_all_elements() -> void
    {
        // Unimplemented until an iterator is implemented
    }

    /**
     * @brief In Managed mode, this sets the tracked initialization value of every field to `false`, effectively
     * resetting the array back to a fresh state, without calling any destructors. No-op in Unmanaged mode.
     *
     * @note This function should be used to 'cancel' an implementation's destructor cleanup logic while in Managed
     * mode. Calling this removes the container's ability to know which elements need to be uninitialized.
     */
    auto discard_all_elements() -> void
    {
        // Unimplemented until an iterator is implemented
    }

  private:
    [[nodiscard]] auto element_ptr(size_t index) -> T*
    {
        return reinterpret_cast<T*>(raw_ptr(index)); // NOLINT
    }
    [[nodiscard]] auto element_ptr(size_t index) const -> const T*
    {
        return reinterpret_cast<const T*>(raw_ptr(index)); // NOLINT
    }

    [[nodiscard]] auto laundered_element_ptr(size_t index) -> T*
    {
        return std::launder(element_ptr(index));
    }
    [[nodiscard]] auto laundered_element_ptr(size_t index) const -> const T*
    {
        return std::launder(element_ptr(index));
    }

    /**
     * Iterator Implementation
     */

    template <bool IsConst>
    struct iterator_impl
    {
        using iterator_category = std::input_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        auto operator*() const -> reference {

        };
        auto operator++() -> iterator_impl&;
        auto operator++(int) -> iterator_impl;

      private:
        T* ptr{nullptr};
    };

    static_assert(std::input_iterator<iterator_impl<false>>, "Not a valid input_iterator!");
};

} // namespace daedalus::containers

#endif
