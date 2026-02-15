#ifndef DAEDALUS_MATH_EASING_H
#define DAEDALUS_MATH_EASING_H

#include "daedalus/math/concepts.h"

#include <array>

namespace dae
{
/**
 * @brief Function prototype for functions that apply an 'ease' effect. The usage strategy is to pass a value to this
 * function type in the domain of [0.0, 1.0] and it will map to the range [0.0, 1.0], but with scaling applied.
 */
template <FloatingPoint T>
using EasingFunction = T (*)(const T);

/**
 * @brief A enumeration of all available easing functions.
 */
enum class EasingFunctionType : uint8_t
{
    LINEAR_INTERPOLATE = 0,
    EASE_IN_OUT_QUAD
};
constexpr size_t EASING_FUNCTION_COUNT = 2;

/**
 * @brief The identity function for easing.
 *
 * @param x The value to scale. Domain: [0.0, 1.0]
 *
 * @return The same input value `x`.
 */
template <FloatingPoint T>
constexpr auto linear_interpolate(const T x) -> T
{
    return x;
}

/**
 * @brief The easing function that applies the ease-in-out-quad function.
 *
 * @param x The value to scale. Domain: [0.0, 1.0]
 *
 * @return `x` with the ease-in-out-quad function applied.
 */
template <FloatingPoint T>
constexpr auto ease_in_out_quad(const T x) -> T
{
    if (x < T{0.5})
    {
        return T{2} * x * x;
    }

    const T shifted = T{2} * x - T{2};
    const T squared = shifted * shifted;
    return T{1} - squared / T{2};
}

/**
 * @brief Retrieves the requisite easing function given an enum identifier.
 *
 * @note If the `EasingFunctionType` enumeration is invalid, then `linear_interpolate<T>` (the identity function) will
 * be returned.
 *
 * @param function_type The `EasingFunctionType` to retrieve.
 *
 * @return The `EasingFunction<T>` associated with the `EasingFunctionType` enumeration, templated on the type `T`.
 */
template <FloatingPoint T>
constexpr auto get_easing_function(const EasingFunctionType function_type) -> EasingFunction<T>
{
    constexpr std::array<EasingFunction<T>, EASING_FUNCTION_COUNT> functions{
        linear_interpolate<T>,
        ease_in_out_quad<T>,
    };
    auto selection = static_cast<size_t>(function_type);
    if (selection < EASING_FUNCTION_COUNT)
    {
        return functions[selection];
    }
    return linear_interpolate<T>;
}
} // namespace dae

#endif
