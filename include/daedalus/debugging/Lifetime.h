#ifndef DAEDALUS_DEBUGGING_LIFETIME_H
#define DAEDALUS_DEBUGGING_LIFETIME_H

#include <iostream>

namespace daedalus::debugging
{

class Lifetime
{
  public:
    Lifetime()
    {
        std::cout << "Constructor\n";
    };
    ~Lifetime()
    {
        std::cout << "Destructor\n";
    };
    Lifetime(const Lifetime&)
    {
        std::cout << "Copy Constructor\n";
    }
    auto operator=(const Lifetime&) -> Lifetime&
    {
        std::cout << "Copy Assignment\n";
        return *this;
    }
    Lifetime(Lifetime&&) noexcept
    {
        std::cout << "Move Constructor\n";
    }
    auto operator=(Lifetime&&) noexcept -> Lifetime&
    {
        std::cout << "Move Assignment\n";
        return *this;
    }
};

} // namespace daedalus::debugging

#endif
