#ifndef DAEDALUS_MATH_CONCEPTS_H
#define DAEDALUS_MATH_CONCEPTS_H

#include <concepts>
#include <type_traits>

namespace dae
{
template <typename T>
concept Numeric = std::is_arithmetic_v<T>;

template <typename T>
concept Integral = std::is_integral_v<T>;

template <typename T>
concept FloatingPoint = std::is_floating_point_v<T>;

} // namespace dae

#endif
