![daedalus](./daedalus_banner.png)

---

# Daedalus
Foundational library I use for my projects. Has some header-only utilities, and some 3rd party includes that I often make use of.

Licenses for 3rd party tools are kept under `/licenses`. Everything in the repo is encapsulated under the `daedalus::` namespace, including 3rd party tools.

## `daedalus/timer.h`

| Type | Description |
|------|-------------|
| daedalus::timer::immutable | A timer whose start time is set at creation and cannot be set again. |
| daedalus::timer::resettable | A timer whose start time can be reset arbitrarily. |

## `daedalus/fileio.h`

# third party

## [`daedalus/boost_unordered.hpp`](https://github.com/MikePopoloski/boost_unordered)
