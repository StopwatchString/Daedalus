#ifndef MOSS_TIMER_H
#define MOSS_TIMER_H

#include <chrono>
#include <deque>

namespace moss {
namespace timer {

    static constexpr double NANOSECONDS_PER_MICROSECOND = 1'000.0;
    static constexpr double NANOSECONDS_PER_MILLISECOND = 1'000'000.0;
    static constexpr double NANOSECONDS_PER_SECOND = 1'000'000'000.0;

class Immutable
{
public:
    Immutable()
    {
        start = std::chrono::steady_clock::now();
    }

    [[nodiscard]] double getSeconds() const noexcept
    {
        return (std::chrono::steady_clock::now() - start).count() / NANOSECONDS_PER_SECOND;
    }

    [[nodiscard]] double getMilliseconds() const noexcept
    {
        return (std::chrono::steady_clock::now() - start).count() / NANOSECONDS_PER_MILLISECOND;
    }

    [[nodiscard]] double getMicroseconds() const noexcept
    {
        return (std::chrono::steady_clock::now() - start).count() / NANOSECONDS_PER_MICROSECOND;
    }

    [[nodiscard]] int64_t getNanoseconds() const noexcept
    {
        return (std::chrono::steady_clock::now() - start).count();
    }

    Immutable(const Immutable& ) = default;
    Immutable(Immutable&& ) = default;
    Immutable& operator=(const Immutable &) = default;
    Immutable& operator=(Immutable &&) = default;

private:
    std::chrono::steady_clock::time_point start{};
};

class Resettable
{
public:
    Resettable()
    {
        start = std::chrono::steady_clock::now();
    }

    void reset() noexcept
    {
        start = std::chrono::steady_clock::now();
    }

    [[nodiscard]] double getSeconds() const noexcept
    {
        return (std::chrono::steady_clock::now() - start).count() / NANOSECONDS_PER_SECOND;
    }

    [[nodiscard]] double getMilliseconds() const noexcept
    {
        return (std::chrono::steady_clock::now() - start).count() / NANOSECONDS_PER_MILLISECOND;
    }

    [[nodiscard]] double getMicroseconds() const noexcept
    {
        return (std::chrono::steady_clock::now() - start).count() / NANOSECONDS_PER_MICROSECOND;
    }

    [[nodiscard]] int64_t getNanoseconds() const noexcept
    {
        return (std::chrono::steady_clock::now() - start).count();
    }

    Resettable(const Resettable&) = default;
    Resettable(Resettable&&) = default;
    Resettable& operator=(const Resettable&) = default;
    Resettable& operator=(Resettable&&) = default;

private:
    std::chrono::steady_clock::time_point start{};
};

template <double _Seconds>
requires (_Seconds > 0.0)
class Rolling
{
public:




    Rolling(const Rolling&) = default;
    Rolling(Rolling&&) = default;
    Rolling& operator=(const Rolling&) = default;
    Rolling& operator=(Rolling&&) = default;

private:
    void trim() {
        const std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
        const auto cutoff = now - std::chrono::duration<double>(_Seconds);

        while (((now - samples.front().first).count() / NANOSECONDS_PER_SECOND) > _Seconds) {
            samples.pop_front();
        }
    }

    std::deque<std::pair<std::chrono::steady_clock::time_point, double>> samples;
};

} // end timer
} // end moss

#endif
