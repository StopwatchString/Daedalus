#ifndef DAEDALUS_DEBUGGING_LIFETIME_H
#define DAEDALUS_DEBUGGING_LIFETIME_H

#include <iostream>

namespace dae
{

template <size_t id = 0>
class Lifetime
{
  public:
    Lifetime()
    {
        std::cout << std::format("{}:Constructor\n", id);
    };
    ~Lifetime()
    {
        std::cout << std::format("{}:Destructor\n", id);
    };
    Lifetime(const Lifetime&)
    {
        std::cout << std::format("{}:Copy Constructor\n", id);
    }
    auto operator=(const Lifetime&) -> Lifetime&
    {
        std::cout << std::format("{}:Copy Assignment\n", id);
        return *this;
    }
    Lifetime(Lifetime&&) noexcept
    {
        std::cout << std::format("{}:Move Constructor\n", id);
    }
    auto operator=(Lifetime&&) noexcept -> Lifetime&
    {
        std::cout << std::format("{}:Move Assignment\n", id);
        return *this;
    }
};

template <size_t id = 0>
class LifetimeCounter
{
  public:
    struct LifetimeCounts
    {
        size_t default_constructor = 0;
        size_t destructor = 0;
        size_t copy_constructor = 0;
        size_t copy_assignment = 0;
        size_t move_constructor = 0;
        size_t move_assignment = 0;
    };
    static LifetimeCounts counts;

    static auto get() -> LifetimeCounts&
    {
        return counts;
    }

    LifetimeCounter()
    {
        get().constructor++;
    };
    ~LifetimeCounter()
    {
        get().destructor++;
        std::cout << "Destructor\n";
    };
    LifetimeCounter(const LifetimeCounter&)
    {
        get().copy_constructor++;
        std::cout << "Copy Constructor\n";
    }
    auto operator=(const LifetimeCounter&) -> LifetimeCounter&
    {
        get().copy_assignment++;
        return *this;
    }
    LifetimeCounter(LifetimeCounter&&) noexcept
    {
        get().move_constructor++;
        std::cout << "Move Constructor\n";
    }
    auto operator=(LifetimeCounter&&) noexcept -> LifetimeCounter&
    {
        get().move_assignment++;
        return *this;
    }
};

} // namespace dae

#endif
