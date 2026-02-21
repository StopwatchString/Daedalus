#ifndef DAEDALUS_CONTAINERS_DELETION_STACK_H
#define DAEDALUS_CONTAINERS_DELETION_STACK_H

#include <functional>
#include <optional>
#include <vector>

namespace dae
{

class FunctionStack
{
  public:
    FunctionStack() = default;
    ~FunctionStack() = default;

    FunctionStack(const FunctionStack& other) = delete;
    auto operator=(const FunctionStack& other) -> FunctionStack& = delete;

    FunctionStack(FunctionStack&& other) noexcept = default;
    auto operator=(FunctionStack&& other) noexcept -> FunctionStack& = default;

    [[nodiscard]] auto size() const -> size_t;

    auto push(const std::function<void()>& deletion_function) -> void;
    auto pop() -> std::optional<std::function<void()>>;
    auto clear() -> void;

    auto execute_last_in_first_out() -> void;
    auto execute_first_in_first_out() -> void;

  private:
    std::vector<std::function<void()>> functions;
};
} // namespace dae

#endif
