#ifndef DAEDALUS_MATH_SMOOTH_VALUE_H
#define DAEDALUS_MATH_SMOOTH_VALUE_H

#include "daedalus/math/concepts.h"
#include "daedalus/math/easing.h"

#include <algorithm>
#include <span>

namespace dae::smoothvalue
{

template <Numeric ValType, FloatingPoint DurationType = double>
struct Data
{
    ValType current_value{};
    ValType start_value{};
    ValType end_value{};
    DurationType duration{DurationType{0}};
    DurationType elapsed{DurationType{0}};
    EasingFunction<DurationType> func{linear_interpolate<DurationType>};
    bool completed{false};
};

template <Numeric ValType, FloatingPoint DurationType>
auto reset(Data<ValType, DurationType>& data, ValType x = ValType{0}) -> void
{
    data.current_value = x;
}

template <Numeric ValType, FloatingPoint DurationType>
auto reset_bulk(std::span<Data<ValType, DurationType>> data_span, ValType x = ValType{0}) -> void
{
    std::ranges::for_each(data_span, [x](Data<ValType, DurationType>& data) -> void { reset(data, x); });
}

template <Numeric ValType, FloatingPoint DurationType>
auto target(Data<ValType, DurationType>& data,
            ValType target,
            DurationType length,
            EasingFunction<DurationType> easing_function) -> void
{
    data.completed = false;
    data.func = easing_function;

    data.start_value = data.current_value;
    data.end_value = target;

    data.elapsed = DurationType{0};
    data.duration = length;
}

template <Numeric ValType, FloatingPoint DurationType>
auto target_bulk(std::span<Data<ValType, DurationType>> data_span,
                 ValType target,
                 DurationType length,
                 EasingFunction<DurationType> easing_function) -> void
{
    std::ranges::for_each(data_span, [&](Data<ValType, DurationType>& data) -> void {
        target(data, target, length, easing_function);
    });
}

template <Numeric ValType, FloatingPoint DurationType>
auto timestep(Data<ValType, DurationType>& data, DurationType dt) -> void
{
    if (!data.completed)
    {
        data.elapsed += dt;
        if (data.elapsed > data.duration)
        {
            data.completed = true;
            data.elapsed = data.duration;
            data.current_value = data.end_value;
        }
        else
        {
            data.current_value =
                (data.end_value - data.start_value) * data.func(data.elapsed / data.duration) + data.start_value;
        }
    }
}

template <Numeric ValType, FloatingPoint DurationType>
auto timestep_bulk(std::span<Data<ValType, DurationType>> data_span, DurationType dt) -> void
{
    std::ranges::for_each(data_span, [&](Data<ValType, DurationType>& data) -> void { timestep(data, dt); });
}

} // namespace dae::smoothvalue

#endif
