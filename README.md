![daedalus](./daedalus_banner.png)

---

# Daedalus

Library chock-full of random utilities and abstractions to make C++ more convenient for me.

# Requirements

`C++20` is required to use this library. This requirement will likely be lifted to `C++23` at some point in the near future.

# How to Use

## Adding to your project

The only build system that will be natively supported is CMake. To include this library using CMake, just call

`add_subdirectory(daedalus)`

The exported target for the library is `daedalus::daedalus`.

If you don't want to hook up via CMake, then it's relatively easy to just rip out whatever pieces are needed. The tools in this library are not exceptionally interdependent.

## Using once in your project

The simplest way to get access to anything in the project is to just use the glob header- `#include "daedalus/daedalus.h"`. This is a little messy though, so if you just want to target what you need, then you can search via the `daedalus/` folder in your includes.

Every tool in Daedalus is sorted parallel to the table of contents below.

# Library Features

Below is a high level introduction to the tools in this library. For a technical reference, use the doxygen docs.

## Table of Contents

- [Containers](#containers)
    - [Array Interface](#array-interface)
    - [Vector Interface](#vector-interface)
    - [Container Settings](#container-settings)
    - [Heap Array](#heap-array)
    - [Heap Vector](#heap-vector)
    - [Stack Array](#stack-array)
    - [Stack Vector](#stack-vector)
    - [Hybrid Vector](#hybrid-vector)
    - [Triple Buffer](#triple-buffer)
- [Core](#core)
    - [Attributes](#attributes)
- [Debugging](#debugging)
    - [Lifetime](#lifetime)
- [IO](#io)
    - [File](#file)
- [Math](#math)
    - [Concepts](#concepts)
    - [Easing](#easing)
    - [Smooth Value](#smooth-value)
- [Platform](#platform)
    - [Linux](#linux)
    - [Windows](#windows)
        - [Selectors](#selectors)
        - [Windows Terminal](#windows-terminal)
- [Profiling](#profiling)
    - [Timer](#timer)
- [Program](#program)
    - [Meta](#meta)
- [Strings](#strings)
    - [Utils](#string-utils)
- [Threading](#threading)
    - [Utils](#thread-utils)

## Containers

Daedalus has in-progress implementations for special explicit API containers that do not hide where their data is stored.

### Array Interface

`#include "daedalus/containers/array_interface.h"`

Abstract Interface for Array containers that do not require initialization of all elements. For the purposes of Daedalus, an `Array` is a container for `N` elements that cannot change size.

### Vector Interface

`#include "daedalus/containers/vector_interface.h"`

Abstract Interface for Vector containers based on the `Array Interface`. For the purposes of Daedalus, a `Vector` is a container for elements that can change in size, but implementations are not required to be able to grow unbounded.

### Container Settings

`#include "daedalus/containers/container_settings.h"`

A handful of trait enumerations used to configure `Array Interface` and `Vector Interface` implementations.

### Heap Array

`#include "daedalus/containers/heap_array.h"`

An implementation of `Array Interface` that puts its data in a heap allocation.

### Heap Vector

`#include "daedalus/containers/heap_vector.h"`

An implementation of `Vector Interface` that puts its data in a heap allocation.

### Stack Array

`#include "daedalus/containers/stack_array.h"`

An implementation of `Array Interface` that puts its data on the stack.

### Stack Vector

`#include "daedalus/containers/stack_vector.h"`

An implementation of `Vector Interface` that puts its data on the stack. This is a capability not in the `STL` until `C++26` via `std::inplace_vector`.

### Hybrid Vector

`#include "daedalus/containers/hybrid_vector.h"`

An implementation of `Vector Interface` that initially stores its data on the stack, but if it grows beyond a compile-time defined bound, then it will reallocate to the heap. The user is able to check this status.

### Triple Buffer

`#include "daedalus/containers/triple_buffer.h"`

An implementation of the `TripleBuffer` pattern in a few flavors. Uses `std::hardware_destructive_interference` to control for false sharing.

`TripleBuffer<T>` only accounts for false-sharing in control structures but not the data itself. This allows for 0 space waste on the value array.

`ZeroShareTripleBuffer<T>` separates both control structures and the data of the triple buffer via `std::hardware_destructive_interference`. This forces the value stored in the buffer to have the alignment of `std::hardware_destructive_interference`, which means wasted space is equivalent to `std::hardware_destructive_interference - alignof(T) * 3` - once for each instance of the value in the buffer. This variant gives by far the best speed results, but take care to pack data towards the size of `std::hardware_destructive_interference` to minimize wasted space. It goes without saying that if you waste space here, you may be hurting cache locality in a way that burns the extra speed of `ZeroShare`.

`(ZeroShare)TripleBuffer(Reader/Writer)` are control structures that can hold a reference to a `TripleBuffer` of any kind and limit the usability of the buffer to either reading or writing. This limits the surface area to make mistakes in code that works with buffers.

## Core

### Attributes

`#include "daedalus/core/attributes.h"`

Currently just attributes taken from the Google `Abseil` library. These are useful for intercompiler code markings.

## Debugging

### Lifetime

`#include "daedalus/debugging/lifetime.h"`

Includes `Lifetime` classes which print and/or track whenever any of the typical object lifetime operations happen to them

- Construction
- Destruction
- Copy Construction
- Copy Assignment
- Move Construction
- Move Assignment

These can be immensely helpful when trying to debug when a data structure or function call is creating/copying/moving your objects when you didn't realize.

## IO

### File

`#include "daedalus/io/file.h"`

Exposes tools for loading files into process-local buffers using different backend strategies, including a helper to request a file async by kicking off a thread and returning a future. Currently supports

- STL built in file loading. Currently does not properly support async, and will just load the file normally when the async future is checked.
- Windows Virtual Filesystem Cached load. Typically the same as STL, but using the Windows Overlapped IO API is able to be dispatched async.
- Windows 'Safe' Direct Disk. This is the most interesting variant currently- uses Windows Overlapped IO to load the file, but does so while bypassing the Virtual Filesystem Cache. This option always reaches directly out to disk. It's not always faster than using the virtual filesystem cache, but it will never be hit by Windows file cache miss penalties. The 'Safe' moniker is because it does not rely on any additional tricks, such as `DirectStorage`, and so should always be available.

## Math

### Concepts

`#include "daedalus/math/concepts.h"`

Common `concepts` (in the `type_traits` sense) for use in math tools.

### Easing

`#include "daedalus/math/easing.h"`

A repository of easing functions. Easing functions can be defined as functions that map \[0.0,1.0] -> \[0.0,1.0] but with special interpolation features. They are most useful when needing to animate motion or any value change in a more organic fashion than just linear interpolation.

### Smooth Value

`#include "daedalus/math/smoothvalue.h"`

An application of easing functions. The user creates any number of `dae::smoothvalue::Data` structs which represent a value in time. Then they call `dae::smoothvalue::target()` on these structs to set a future target value, along with the amount of time to take to get to that value, and what kind of easing function to use to get there. Then the user can progressively apply `dt` to their data using `dae::smoothvalue::timestep()`, and use the `current_value` field to get the value as it changes.

This tool has associated bulk processing functions for working with many `dae::smoothvalue::Data` instances at a time, though they just rely on `std::ranges::for_each"` for parallization, so speedups compared to other solutions are unkown.

## Platform

### Linux

Nothing `Linux` specific for now.

### Windows

#### Selectors

`#include "daedalus/platform/windows/selectors.h"`

Currently just a simple abstraction over the `Windows` filesystem picker dialogue to spawn the window and return the selected file as a `std::string`.

#### Windows Terminal

`#include "daedalus/platform/windows/terminal.h"`

A bunch of utility functions for querying and configuring the terminal in `Windows`. The terminal in `Windows` is generally quite incapable but `Windows Terminal` (the application) has many of the features that are typically expected in `Linux` terminals for color and text control.

## Profiling

### Timer

`#include "daedalus/profiling/timer.h"`

Timer abstractions to make it more intuitive to get the results in the timescale needed.

## Program

### Meta

`#include "daedalus/program/meta.h"`

A collection of tools to retrieve meta information about your program and the environment from the OS. Includes a special `get_program_meta(int argc, char** argv)` function that returns a `ProgramMeta` struct, which can be used at the beginning of a program to quickly make available all of the metadata that this function can query for.

## Strings

### String Utils

`#include "daedalus/strings/utils.h"`

Includes a bunch of core string utilities that make working with strings and files way simpler.

- `get_line()`
- `split()`
- `trim()`
- `is_all_whitespace()`
- `to_wide()`
- `from_wide()`
- `get_line_wide()`
- `split_wide()`

## Threading

### Thread Utils

`#include "daedalus/threading/utils.h"`

Abstracts some functions for working with and configuring threads that are platform-dependent but aren't included in the STL's implementation of threads.

- `set_thread_logical_processor_affinity()`
- `set_this_thread_logical_processor_affinity()`
- `set_thread_name()`
