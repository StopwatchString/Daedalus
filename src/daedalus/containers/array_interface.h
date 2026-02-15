#ifndef DAEDALUS_CONTAINERS_ARRAY_INTERFACE_H
#define DAEDALUS_CONTAINERS_ARRAY_INTERFACE_H

#include "daedalus/containers/container_settings.h"

#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>

namespace daedalus::containers
{

/**
 * @brief An array interface that focuses on explicit API and data interaction. The interface can be implemented with
 * any choice of underlying contiguous data source via implementation of a handful of CRTP-exposed functions, documented
 * at the top of the interface.
 * The interface does not enforce initialization of its elements. It uses `std::construct_at()` and `std::destroy_at()`
 * at index locations in the container rather than assignment operations, meaning it can skip initialization and
 * destructor penalties if configured to do so.
 * The interface comes with 3 management modes- `Unmanaged`, `Managed`, and `ManagedWithBoundsChecks`.
 * `Unmanaged` mode means that the container does not track which elements have been created or destroyed- that's
 * entirely up to the user. This means that if you place an object in the array and overwrite it with any methods, its
 * destructor will **not** be called for you. It is instead up to the user whether they want to call `destroy_at()`
 * before overwriting or not. This includes when the array itself is destroyed. If you do not manually call destructors
 * on elements, then every element will be discarded without doing so. This can be highly advantageous when you don't
 * want to pay the time cost of destructors. This explicit management behavior is also what allows the array to be
 * constructed without being initialized.
 * `Managed` mode means that the container will track which elements have been created and destroyed. The bookkeeping to
 * track intialized memory in the array is implementation-defined, however it will typically be a boolean array. The
 * container will then automatically call destructors when elements are overidden and when the array is destructed.
 * `ManagedWithBoundsChecks` mode is the same as `Managed`, except the container will throw an exception if an index is
 * passed to it that is outside of its size bounds.
 *
 * @note To comply with the safety that `Managed` mode provides, all getters will create default instances of `T` in the
 * array for indices that are requested but not yet initialized. Because of this requirement, getters that return const
 * types are overloaded between `Managed` and `Unmanaged`, with the `Managed` versions not being `const` functions.
 *
 * @tparam Derived The CRTP injection parameter
 * @tparam T The type the container holds
 * @tparam MMode The ManagementMode of the container. In Unmanaged mode, the container will never manually call a
 * destructor for any element. In Managed mode, the container will call destructors for elements when they are overriden
 * or when the array is destroyed.
 * @tparam ValsForDefaultT Optional value pack that can be forwarded to default constructed `T`s instead of using the
 * default constructor (allowing the usage of non-default constructors for default constructed values). If `T` does not
 * have a default constructor and `ValsForDefaultT` are not provided, then `Managed` mode will not compile as the
 * behavior of getters is undefined when used on an index that points to uninitialized memory.
 */
template <typename Derived, typename T, ManagementMode MMode, auto... ValsForDefaultT>
    requires(MMode == ManagementMode::Unmanaged || std::is_default_constructible_v<T> || sizeof...(ValsForDefaultT) > 0)
class array_interface
{
    /**
     * Implementer's CRTP Interface
     */
  public:
    /**
     * @brief Get a non-const byte pointer for the container's data.
     *
     * @return Pointer to the container's data.
     */
    [[nodiscard]] auto data() -> std::byte*
    {
        return static_cast<Derived*>(this)->data();
    };

    /**
     * @brief Get a const byte pointer for the container's data.
     *
     * @return Const pointer to the container's data.
     */
    [[nodiscard]] auto data() const -> const std::byte*
    {
        return static_cast<const Derived*>(this)->data();
    };

    /**
     * @brief Get the size of the container.
     *
     * @return The size of the container.
     */
    [[nodiscard]] auto size() const -> size_t
    {
        return static_cast<const Derived*>(this)->size();
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
        static_cast<Derived*>(this)->forget_all_elements();
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
        return static_cast<const Derived*>(this)->element_is_init(index);
    }

    /**
     * @brief Set the initialized state of an element for tracking purposes.
     *
     * @param index The index of the element to set init state on.
     * @param init The init state to set on the element.
     */
    auto set_element_init_state(size_t index, bool init) -> void
    {
        static_cast<Derived*>(this)->set_element_init_state(index, init);
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
        return static_cast<Derived*>(this)->raw_ptr(index);
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
        return static_cast<const Derived*>(this)->raw_ptr(index);
    }

    /**
     * Implemented Interface
     */
  public:
    /**
     * @brief Copies data into the container at a given index.
     *
     * @note If container is in `Managed` mode, then this function will first check if the container has noted that an
     * element is already initialized at this location and destroy it before copying the new value into the array. The
     * initialized state of this element location will also be set to `true`.
     *
     * @note If container is in `Unmanaged` mode, then this function will *NOT* call the destructor on any existing
     * object at this location in the array.
     *
     * @param index The index to copy the element into.
     * @param value The value to copy into the container.
     */
    auto copy_to(size_t index, const T& value) -> void
    {
        if constexpr (MMode >= ManagementMode::ManagedWithBoundsChecks)
        {
            assert(index < size());
        }
        if constexpr (MMode >= ManagementMode::Managed)
        {
            if (element_is_init(index))
            {
                std::destroy_at(laundered_element_ptr(index));
            }
        }
        std::construct_at(element_ptr(index), value);
        if constexpr (MMode >= ManagementMode::Managed)
        {
            set_element_init_state(index, true);
        }
    }

    /**
     * @brief Moves data into the container at a given index.
     *
     * @note If container is in `Managed` mode, then this function will first check if the container has noted that an
     * element is already initialized at this location and destroy it before moving the new value into the array. The
     * initialized state of this element location will also be set to `true`.
     *
     * @note If container is in `Unmanaged` mode, then this function will *NOT* call the destructor on any existing
     * object at this location in the array.
     *
     * @param index The index to move the element into.
     * @param value The value to move into the container.
     */
    auto move_to(size_t index, T&& value) -> void
    {
        if constexpr (MMode >= ManagementMode::ManagedWithBoundsChecks)
        {
            assert(index < size());
        }
        if constexpr (MMode >= ManagementMode::Managed)
        {
            if (element_is_init(index))
            {
                std::destroy_at(laundered_element_ptr(index));
            }
        }
        std::construct_at(element_ptr(index), std::move(value));
        if constexpr (MMode >= ManagementMode::Managed)
        {
            set_element_init_state(index, true);
        }
    }

    /**
     * @brief Does an in-place construction of an element at a given index in the container. Arguments for creation
     * are forwarded to a constructor for the element.
     *
     * @note Use this function to avoid unnecessary copies or moves when trying to pass a temporary into the container.
     * EX: `copy_to(0, Foo(1, 2))` or `move_to(0, Foo(1, 2))` -> `create_at(0, 1, 2)`
     *
     * @note If container is in `Managed` mode, then this function will first check if the container has noted that an
     * element is already initialized at this location and destroy it before creating a new value. The initialized state
     * of this element location will also be set to `true`.
     *
     * @note If container is in `Unmanaged` mode, then this function will *NOT* call the destructor on any existing
     * object at this location in the array.
     *
     * @tparam Args Types of the args passed for the constructor.
     *
     * @param index The index to create the element at.
     * @param args Arguments for the element type's constructor, which are transparently forwarded to the constructor.
     */
    template <typename... Args>
    auto create_at(size_t index, Args&&... args) -> void
    {
        if constexpr (MMode >= ManagementMode::ManagedWithBoundsChecks)
        {
            assert(index < size());
        }
        if constexpr (MMode >= ManagementMode::Managed)
        {
            if (element_is_init(index))
            {
                std::destroy_at(laundered_element_ptr(index));
            }
        }

        /**
         * Constexpr select the correct way to default construct the object. First, choose user-provided parameters from
         * create_at() if possible. If they didn't supply arguments, then use ValsForDefaultT provided to the
         * array_interface template as a way to default construct T. This is only needed if the element type T cannot be
         * default constructed. Finally, if neither of those are provided they attempt to default construct the element.
         */
        if constexpr (sizeof...(args) > 0)
        {
            std::construct_at(element_ptr(index), std::forward<Args>(args)...);
        }
        else if constexpr (sizeof...(ValsForDefaultT) > 0)
        {
            std::construct_at(element_ptr(index), ValsForDefaultT...);
        }
        else
        {
            std::construct_at(element_ptr(index));
        }

        if constexpr (MMode >= ManagementMode::Managed)
        {
            set_element_init_state(index, true);
        }
    }

    /**
     * @brief Manually calls the destructor on an element at a given index using `std::destroy_at()`. Use with extreme
     * caution in `Unmanaged` mode.
     *
     * @note  This is both necessary and *VERY* dangerous when working with non primitive types in `Unmanaged` mode. Use
     * with extreme caution. If you call this function on an uninitialized element then `std::destroy_at()` will treat
     * that uninitialized memory like a correctly constructed element type and execute its destructor on that memory.
     * Things *will* blow up if that type has a non trivial destructor. However, this functionality is doubly *required*
     * for `Unmanaged` mode, as the user must be able to destroy elements without the container tracking which are
     * initialized.
     *
     * @note In `Managed` mode, `std::destroy_at()` is only called on the element if it has been marked as initialized.
     * This call also changes the initialized state of that element index to be `false`.
     *
     * @param index The index into the container on which to call `std::destroy_at()`.
     */
    auto destroy_at(size_t index) -> void
    {
        if constexpr (MMode >= ManagementMode::ManagedWithBoundsChecks)
        {
            assert(index < size());
        }
        if constexpr (MMode >= ManagementMode::Managed)
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

    /**
     * @brief Copies the data of an element in the array out of the array.
     *
     * @note This version of the function only applies to `Managed` mode arrays and is non const. If this function is
     * called on an uninitialized element, then a default `T` is constructed at that location using either the default
     * constructor or the values declared as `ValsForDefaultT` on the container.
     *
     * @param index The index of the element to copy.
     *
     * @return A copy of the element in the array.
     */
    auto copy_from(size_t index) -> T
        requires(MMode >= ManagementMode::Managed)
    {
        if constexpr (MMode >= ManagementMode::ManagedWithBoundsChecks)
        {
            assert(index < size());
        }
        if (!element_is_init(index))
        {
            create_at(index);
        }
        return *laundered_element_ptr(index);
    }

    /**
     * @brief Copies the data of an element in the array out of the array.
     *
     * @note This verison of the function only applies to `Unmanaged` mode arrays and is const. Take care not to call it
     * on uninitialized elements in the array.
     *
     * @param index The index of the element to copy.
     *
     * @return A copy of the element in the array.
     */
    auto copy_from(size_t index) const -> T
        requires(MMode == ManagementMode::Unmanaged)
    {
        return *laundered_element_ptr(index);
    }

    /**
     * @brief Moves the data of an element in the array out of the array.
     *
     * @note In `Managed` mode, this function will default construct an element at the given index if the array element
     * is uninitialized.
     *
     * @param index The index of the element to move.
     *
     * @return The moved element from the array.
     */
    auto move_from(size_t index) -> T
    {
        if constexpr (MMode >= ManagementMode::ManagedWithBoundsChecks)
        {
            assert(index < size());
        }
        if constexpr (MMode >= ManagementMode::Managed)
        {
            if (!element_is_init(index))
            {
                create_at(index);
            }
            set_element_init_state(index, false);
        }
        return std::move(*laundered_element_ptr(index));
    }

    /**
     * @brief Returns a non-const reference to an element in the array.
     *
     * @note In `Managed` mode, this function will default construct an element at the given index if the array element
     * is uninitialized.
     *
     * @param index The index of the element to reference.
     *
     * @return A reference to the element in the array.
     */
    auto ref_from(size_t index) -> T&
    {
        if constexpr (MMode >= ManagementMode::ManagedWithBoundsChecks)
        {
            assert(index < size());
        }
        if constexpr (MMode >= ManagementMode::Managed)
        {
            if (!element_is_init(index))
            {
                create_at(index);
            }
        }
        return *laundered_element_ptr(index);
    }

    /**
     * @brief Returns a const reference to an element in the array.
     *
     * @note This version of the function only applies to `Managed` mode arrays and is non const. If this function is
     * called on an uninitialized element, then a default `T` is constructed at that location using either the default
     * constructor or the values declared as `ValsForDefaultT` on the container.
     *
     * @param index The index of the element to reference.
     *
     * @return A reference to the element in the array.
     */
    auto const_ref_from(size_t index) -> const T&
        requires(MMode >= ManagementMode::Managed)
    {
        if constexpr (MMode >= ManagementMode::ManagedWithBoundsChecks)
        {
            assert(index < size());
        }
        if (!element_is_init(index))
        {
            create_at(index);
        }
        return *laundered_element_ptr(index);
    }

    /**
     * @brief Returns a const reference to an element in the array.
     *
     * @note This verison of the function only applies to `Unmanaged` mode arrays and is const. Take care not to call it
     * on uninitialized elements in the array.
     *
     * @param index The index of the element to reference.
     *
     * @return A const reference to the elemnet in the array.
     */
    auto const_ref_from(size_t index) const -> const T&
        requires(MMode == ManagementMode::Unmanaged)
    {
        if constexpr (MMode >= ManagementMode::ManagedWithBoundsChecks)
        {
            assert(index < size());
        }
        return *laundered_element_ptr(index);
    }

    /**
     * @brief Calls std::destroy_at() on all elements of the array. WARNING: In `Unmanaged` mode, this is done
     * indiscriminately and if the contained type has a nontrivial destructor, you're liable to blow something up if the
     * whole array isn't initialized. In `Managed` mode, std::destroy_at() is only called on elements marked as
     * initialized.
     *
     * @note After this function is called, the entire array is assumed to be uninitialized.
     *
     * @note Derived classes should use this to implement their destructor in `Managed` mode.
     */
    auto destroy_all_elements() -> void
    {
        // Unimplemented until an iterator is implemented
    }

  private:
    /**
     *  @brief Returns a non-laundered pointer of type `T` at the given index.
     *
     * @param index The element index to get the pointer to.
     *
     * @return A non-laundered pointer to `T` at the given index.
     */
    [[nodiscard]] auto element_ptr(size_t index) -> T*
    {
        return reinterpret_cast<T*>(raw_ptr(index)); // NOLINT
    }

    /**
     *  @brief Returns a const non-laundered pointer of type `T` at the given index.
     *
     * @param index The element index to get the pointer to.
     *
     * @return A const non-laundered pointer to `T` at the given index.
     */
    [[nodiscard]] auto element_ptr(size_t index) const -> const T*
    {
        return reinterpret_cast<const T*>(raw_ptr(index)); // NOLINT
    }

    /**
     *  @brief Returns a laundered pointer of type `T` at the given index.
     *
     * @param index The element index to get the pointer to.
     *
     * @return A laundered pointer to `T` at the given index.
     */
    [[nodiscard]] auto laundered_element_ptr(size_t index) -> T*
    {
        return std::launder(element_ptr(index));
    }

    /**
     *  @brief Returns a const laundered pointer of type `T` at the given index.
     *
     * @param index The element index to get the pointer to.
     *
     * @return A const laundered pointer to `T` at the given index.
     */
    [[nodiscard]] auto laundered_element_ptr(size_t index) const -> const T*
    {
        return std::launder(element_ptr(index));
    }

    /**
     * Iterator Implementation
     */

    // template <bool IsConst>
    // struct iterator_impl
    // {
    //     using iterator_category = std::input_iterator_tag;
    //     using value_type = T;
    //     using difference_type = std::ptrdiff_t;
    //     using pointer = T*;
    //     using reference = T&;

    //     auto operator*() const -> reference {

    //     };
    //     auto operator++() -> iterator_impl&;
    //     auto operator++(int) -> iterator_impl;

    //   private:
    //     T* ptr{nullptr};
    // };

    // static_assert(std::input_iterator<iterator_impl<false>>, "Not a valid input_iterator!");
};

} // namespace daedalus::containers

#endif
