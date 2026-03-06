#ifndef DAEDALUS_MATH_STATISTICS_H
#define DAEDALUS_MATH_STATISTICS_H

#include <cmath>
#include <numeric>

namespace dae
{

template <std::floating_point TFloat = double, typename TContainer, typename TSelector = std::identity>
auto standard_deviation(const TContainer& values, TSelector selector = {}) -> TFloat
{
    if (values.empty())
    {
        return TFloat{0};
    }

    TFloat selected_sum =
        std::accumulate(values.begin(), values.end(), TFloat{0}, [&selector](TFloat acc, const auto& val) -> TFloat {
            return acc + static_cast<TFloat>(selector(val));
        });

    TFloat mean = selected_sum / static_cast<TFloat>(values.size());

    TFloat variance = std::accumulate(values.begin(),
                                      values.end(),
                                      TFloat{0},
                                      [&selector, mean](TFloat acc, const auto& val) -> TFloat {
                                          TFloat diff = static_cast<TFloat>(selector(val)) - mean;
                                          return acc + diff * diff;
                                      }) /
                      static_cast<TFloat>(values.size());

    return std::sqrt(variance);
}

} // namespace dae

#endif
