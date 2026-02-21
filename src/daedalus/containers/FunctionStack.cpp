#include "daedalus/containers/FunctionStack.h"

namespace dae
{
auto FunctionStack::size() const -> size_t
{
    return functions.size();
}

auto FunctionStack::push(const std::function<void()>& function)
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
    for (size_t i = functions.size() - 1; i >= 0; i--)
    {
        functions[i]();
    }
}
auto FunctionStack::execute_first_in_first_out() -> void
{
    for (size_t i = 0; i < functions.size(); i++)
    {
        functions[i]();
    }
}
} // namespace dae
