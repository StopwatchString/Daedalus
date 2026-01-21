#ifndef DAEDALUS_TRIPLE_BUFFER_H
#define DAEDALUS_TRIPLE_BUFFER_H

#include "daedalus/types.h"

#include <atomic>
#include <new>
#include <utility>

template <typename T>
class TripleBuffer
{

  public:
    auto get_for_writer() -> T&
    {
        return buffers[back_idx];
    }

    void publish()
    {
        State new_spare{back_idx, true};
        State prev_spare = spare.exchange(new_spare, std::memory_order_acq_rel);
        back_idx = prev_spare.idx;
    }

    auto get_for_reader() -> std::pair<T&, bool>
    {
        State curr_spare = spare.load(std::memory_order_relaxed);
        bool updated = curr_spare.has_update;
        if (updated)
        {
            State new_spare{front_idx, false};
            State prev_spare = spare.exchange(new_spare, std::memory_order_acq_rel);
            front_idx = prev_spare.idx;
        }
        return {buffers[front_idx], updated};
    }

  private:
    struct State
    {
        daedalus::primitives::u32 idx{0};
        bool has_update{false};
    };
    static_assert(std::atomic<State>::is_always_lock_free);

    T buffers[3];
    alignas(std::hardware_destructive_interference_size) daedalus::primitives::u64 front_idx{0};
    alignas(std::hardware_destructive_interference_size) std::atomic<State> spare{{1, false}};
    alignas(std::hardware_destructive_interference_size) daedalus::primitives::u64 back_idx{2};
};

template <typename T>
class TripleBufferReader
{
  public:
    TripleBufferReader(TripleBuffer<T>& triple_buffer) : instance(triple_buffer) {};

    auto read() -> std::pair<T&, bool>
    {
        return instance.get_for_reader();
    }

  private:
    TripleBuffer<T>& instance;
};

template <typename T>
class TripleBufferWriter
{
  public:
    TripleBufferWriter(TripleBuffer<T>& triple_buffer) : instance(triple_buffer) {};

    template <class U>
    void write(U&& val)
    {
        instance.get_for_writer() = std::forward<U>(val);
        instance.publish();
    }

  private:
    TripleBuffer<T>& instance;
};

template <typename T>
class ZeroShareTripleBuffer
{
    struct CacheLineAlignedData
    {
        alignas(std::hardware_destructive_interference_size) T value;
        std::byte padding[std::hardware_destructive_interference_size -
                          (sizeof(T) % std::hardware_destructive_interference_size) %
                              std::hardware_destructive_interference_size];
    };

  public:
    auto get_for_writer() -> T&
    {
        return buffers[back_idx].value;
    }

    void publish()
    {
        State new_spare{back_idx, true};
        State prev_spare = spare.exchange(new_spare, std::memory_order_acq_rel);
        back_idx = prev_spare.idx;
    }

    auto get_for_reader() -> std::pair<T&, bool>
    {
        State curr_spare = spare.load(std::memory_order_relaxed);
        bool updated = curr_spare.has_update;
        if (updated)
        {
            State new_spare{front_idx, false};
            State prev_spare = spare.exchange(new_spare, std::memory_order_acq_rel);
            front_idx = prev_spare.idx;
        }
        return {buffers[front_idx].value, updated};
    }

  private:
    struct State
    {
        daedalus::primitives::u32 idx{0};
        bool has_update{false};
    };
    static_assert(std::atomic<State>::is_always_lock_free);

    CacheLineAlignedData buffers[3];
    alignas(std::hardware_destructive_interference_size) daedalus::primitives::u64 front_idx{0};
    alignas(std::hardware_destructive_interference_size) std::atomic<State> spare{{1, false}};
    alignas(std::hardware_destructive_interference_size) daedalus::primitives::u64 back_idx{2};
};

template <typename T>
class ZeroShareTripleBufferReader
{
  public:
    ZeroShareTripleBufferReader(ZeroShareTripleBuffer<T>& triple_buffer) : instance(triple_buffer) {};

    auto read() -> std::pair<T&, bool>
    {
        return instance.get_for_reader();
    }

  private:
    ZeroShareTripleBuffer<T>& instance;
};

template <typename T>
class ZeroShareTripleBufferWriter
{
  public:
    ZeroShareTripleBufferWriter(ZeroShareTripleBuffer<T>& triple_buffer) : instance(triple_buffer) {};

    template <typename U>
    void write(U&& val)
    {
        instance.get_for_writer() = std::forward<U>(val);
        instance.publish();
    }

  private:
    ZeroShareTripleBuffer<T>& instance;
};

#endif
