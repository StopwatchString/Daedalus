#ifndef DAEDALUS_PROFILING_FRAME_PROFILER_H
#define DAEDALUS_PROFILING_FRAME_PROFILER_H

#include "daedalus/math/statistics.h"

#include <chrono>
#include <cstdlib>
#include <deque>
#include <limits>
#include <queue>

namespace dae
{
static constexpr double NANOSECONDS_PER_MICROSECOND = 1'000.0;
static constexpr double NANOSECONDS_PER_MILLISECOND = 1'000'000.0;
static constexpr double NANOSECONDS_PER_SECOND = 1'000'000'000.0;

struct FrameStatistics
{
    std::chrono::steady_clock::time_point earliest_start_time{};
    std::chrono::steady_clock::time_point latest_end_time{};

    std::chrono::steady_clock::duration average_elapsed_ns{};
    std::chrono::steady_clock::duration p90_elapsed_ns{};
    std::chrono::steady_clock::duration p95_elapsed_ns{};
    std::chrono::steady_clock::duration p99_elapsed_ns{};
    std::chrono::steady_clock::duration min_elapsed_ns{std::numeric_limits<long long>::max()};
    std::chrono::steady_clock::duration max_elapsed_ns{std::numeric_limits<long long>::min()};
    std::chrono::steady_clock::duration std_deviation_elapsed_ns{};
    std::chrono::steady_clock::duration average_frame_to_frame_jitter_ns{};

    size_t samples{0};

    [[nodiscard]] auto total_time_elapsed_seconds() const -> double
    {
        return static_cast<double>((latest_end_time - earliest_start_time).count()) / NANOSECONDS_PER_SECOND;
    }

    [[nodiscard]] auto total_time_elapsed_ms() const -> double
    {
        return static_cast<double>((latest_end_time - earliest_start_time).count()) / NANOSECONDS_PER_MILLISECOND;
    }

    [[nodiscard]] auto total_time_elapsed_us() const -> double
    {
        return static_cast<double>((latest_end_time - earliest_start_time).count()) / NANOSECONDS_PER_MICROSECOND;
    }

    [[nodiscard]] auto total_time_elapsed_ns() const -> int64_t
    {
        return (latest_end_time - earliest_start_time).count();
    }

    [[nodiscard]] auto frames_per_second() const -> double
    {
        return 1.0 / (static_cast<double>(average_elapsed_ns.count()) / NANOSECONDS_PER_SECOND);
    }

    [[nodiscard]] auto frames_per_second_p99() const -> double
    {
        return 1.0 / (static_cast<double>(p99_elapsed_ns.count()) / NANOSECONDS_PER_SECOND);
    }

    [[nodiscard]] auto frames_per_second_p95() const -> double
    {
        return 1.0 / (static_cast<double>(p95_elapsed_ns.count()) / NANOSECONDS_PER_SECOND);
    }

    [[nodiscard]] auto frames_per_second_p90() const -> double
    {
        return 1.0 / (static_cast<double>(p90_elapsed_ns.count()) / NANOSECONDS_PER_SECOND);
    }

    [[nodiscard]] auto average_frametime_ms() const -> double
    {
        return static_cast<double>(average_elapsed_ns.count()) / NANOSECONDS_PER_MILLISECOND;
    }

    [[nodiscard]] auto p99_frametime_ms() const -> double
    {
        return static_cast<double>(p99_elapsed_ns.count()) / NANOSECONDS_PER_MILLISECOND;
    }

    [[nodiscard]] auto p95_frametime_ms() const -> double
    {
        return static_cast<double>(p95_elapsed_ns.count()) / NANOSECONDS_PER_MILLISECOND;
    }

    [[nodiscard]] auto p90_frametime_ms() const -> double
    {
        return static_cast<double>(p90_elapsed_ns.count()) / NANOSECONDS_PER_MILLISECOND;
    }

    [[nodiscard]] auto min_frametime_ms() const -> double
    {
        return static_cast<double>(min_elapsed_ns.count()) / NANOSECONDS_PER_MILLISECOND;
    }

    [[nodiscard]] auto max_frametime_ms() const -> double
    {
        return static_cast<double>(max_elapsed_ns.count()) / NANOSECONDS_PER_MILLISECOND;
    }

    [[nodiscard]] auto standard_deviation_frametime_ms() const -> double
    {
        return static_cast<double>(std_deviation_elapsed_ns.count()) / NANOSECONDS_PER_MILLISECOND;
    }

    [[nodiscard]] auto average_frame_to_frame_jitter_ms() -> double
    {
        return static_cast<double>(average_frame_to_frame_jitter_ns.count()) / NANOSECONDS_PER_MILLISECOND;
    }

    [[nodiscard]] auto sample_count() const -> size_t
    {
        return samples;
    }
};

class FrameProfiler
{
    struct Frame
    {
        std::chrono::steady_clock::time_point start{};
        std::chrono::steady_clock::time_point end{};

        auto operator<=>(const Frame& other) const
        {
            return (end - start) <=> (other.end - other.start);
        }
    };

  public:
    explicit FrameProfiler() = default;
    ~FrameProfiler() = default;

    FrameProfiler(const FrameProfiler& other) = default;
    auto operator=(const FrameProfiler& other) -> FrameProfiler& = default;

    FrameProfiler(FrameProfiler&& other) noexcept = default;
    auto operator=(FrameProfiler&& other) noexcept -> FrameProfiler& = default;

    auto start() -> void
    {
        frame_in_flight = Frame();
        frame_in_flight.start = std::chrono::steady_clock::now();
    }

    auto end() -> void
    {
        frame_in_flight.end = std::chrono::steady_clock::now();
        frames.push_back(frame_in_flight);
    }

    auto size() -> size_t
    {
        return frames.size();
    }

    auto clear() -> void
    {
        frames.clear();
    }

    auto frames_ref() -> const std::deque<Frame>&
    {
        return frames;
    }

    auto retain_last(std::chrono::seconds seconds) -> void
    {
        if (!frames.empty())
        {
            std::chrono::steady_clock::time_point cutoff = frames.back().end - seconds;
            while (frames.front().end < cutoff)
            {
                frames.pop_front();
            }
        }
    }

    auto get_statistics() -> FrameStatistics
    {
        FrameStatistics frame_statistics{};

        if (frames.empty())
        {
            return frame_statistics;
        }

        // Main timespan
        frame_statistics.earliest_start_time = frames.front().start;
        frame_statistics.latest_end_time = frames.back().end;

        // Percentile Calculations
        std::priority_queue<Frame> heap;
        for (const Frame frame : frames)
        {
            heap.push(frame);
        }

        static constexpr double P99_RATIO = 100.0;
        static constexpr double P95_RATIO = 20.0;
        static constexpr double P90_RATIO = 10.0;

        size_t p99_count = static_cast<size_t>(static_cast<double>(frames.size()) / P99_RATIO);
        size_t p95_count = static_cast<size_t>(static_cast<double>(frames.size()) / P95_RATIO);
        size_t p90_count = static_cast<size_t>(static_cast<double>(frames.size()) / P90_RATIO);
        size_t count = 0;

        while (count < p99_count)
        {
            count++;
            heap.pop();
        }
        frame_statistics.p99_elapsed_ns = heap.top().end - heap.top().start;

        while (count < p95_count)
        {
            count++;
            heap.pop();
        }
        frame_statistics.p95_elapsed_ns = heap.top().end - heap.top().start;

        while (count < p90_count)
        {
            count++;
            heap.pop();
        }
        frame_statistics.p90_elapsed_ns = heap.top().end - heap.top().start;

        // Standard Deviation
        double std_deviation =
            dae::standard_deviation(frames, [](const Frame& f) -> int64_t { return (f.end - f.start).count(); });
        frame_statistics.std_deviation_elapsed_ns =
            std::chrono::steady_clock::duration(static_cast<int64_t>(std_deviation));

        // Min/Max Average
        if (!frames.empty())
        {
            std::chrono::steady_clock::duration total{};
            for (const Frame& frame : frames)
            {
                std::chrono::steady_clock::duration d = frame.end - frame.start;
                total += frame.end - frame.start;
                frame_statistics.max_elapsed_ns = std::max(frame_statistics.max_elapsed_ns, d);
                frame_statistics.min_elapsed_ns = std::min(frame_statistics.min_elapsed_ns, d);
            }
            frame_statistics.average_elapsed_ns = total / frames.size();
        }

        // Jitter
        if (frames.size() > 1)
        {
            auto it = frames.begin();
            auto follow_it = frames.begin();

            ++it;

            std::chrono::steady_clock::duration total_diff{};
            size_t diffs_counted = 0;

            while (it != frames.end())
            {
                std::chrono::steady_clock::duration current = (*it).end - (*it).start;
                std::chrono::steady_clock::duration previous = (*follow_it).end - (*follow_it).start;

                total_diff += std::chrono::abs(current - previous);
                diffs_counted++;

                ++it;
                ++follow_it;
            }

            frame_statistics.average_frame_to_frame_jitter_ns = total_diff / diffs_counted;
        }

        return frame_statistics;
    }

  private:
    // TODO::IMPROVE std::deque<> is not ideal for performance
    std::deque<Frame> frames;
    Frame frame_in_flight{};
};
} // namespace dae

#endif
