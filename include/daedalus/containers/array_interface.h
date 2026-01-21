#ifndef DAEDALUS_CONTAINERS_ARRAY_INTERFACE_H
#define DAEDALUS_CONTAINERS_ARRAY_INTERFACE_H

#include "daedalus/containers/container_settings.h"

#include <cstddef>
#include <memory>
#include <utility>

namespace daedalus::containers
{

template <typename Derived, typename T, ManagementMode MMode>
class array_interface
{
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

    [[nodiscard]] auto data() -> std::byte*
    {
        return static_cast<Derived*>(this)->data();
    };

    [[nodiscard]] auto size() const -> size_t
    {
        return static_cast<Derived*>(this)->size();
    }

  private:
    [[nodiscard]] auto element_is_init(size_t index) -> bool
    {
        return static_cast<Derived*>(this)->element_is_init(index);
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
        return static_cast<Derived*>(this)->raw_ptr(index);
    }

    [[nodiscard]] auto element_ptr(size_t index) -> T*
    {
        return reinterpret_cast<T*>(raw_ptr(index)); // NOLINT
    }
    [[nodiscard]] auto element_ptr(size_t index) const -> const T*
    {
        return reinterpret_cast<T*>(raw_ptr(index)); // NOLINT
    }

    [[nodiscard]] auto laundered_element_ptr(size_t index) -> T*
    {
        return std::launder(element_ptr(index));
    }
    [[nodiscard]] auto laundered_element_ptr(size_t index) const -> const T*
    {
        return std::launder(element_ptr(index));
    }
};

} // namespace daedalus::containers

#endif
