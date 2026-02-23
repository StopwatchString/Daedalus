#include "daedalus/containers/FunctionStack.h"

#include <ranges>

namespace dae
{
auto FunctionStack::size() const -> size_t
{
    return functions.size();
}

auto FunctionStack::push(const std::function<void()>& function) -> void
{
    functions.push_back(function);
}

auto FunctionStack::pop() -> std::optional<std::function<void()>>
{
    if (functions.size() == 0)
    {
        return std::nullopt;
    }

    std::function<void()> function = functions.back();
    functions.pop_back();
    return function;
}

auto FunctionStack::clear() -> void
{
    functions.clear();
}

auto FunctionStack::execute_last_in_first_out() -> void
{
    for (std::function<void()>& f : functions | std::views::reverse)
    {
        f();
    }
}

auto FunctionStack::execute_first_in_first_out() -> void
{
    for (std::function<void()>& f : functions)
    {
        f();
    }
}
} // namespace dae
