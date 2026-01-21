#ifndef DAEDALUS_CONTAINERS_CONTAINER_SETTINGS_H
#define DAEDALUS_CONTAINERS_CONTAINER_SETTINGS_H

#include <cstdint>

namespace daedalus::containers
{

enum class ManagementMode : uint8_t
{
    // Container should not manage anything for the user. Destructors will not be called.
    Manual,
    // Container will track what elements have been populated and call destructors on overwrite and on container
    // destructor.
    Managed
};

// enum class InitializationTrackingMode : uint8_t
// {
//     // Use containers of bools to track initialization state. Faster setting and lookup, more memory usage.
//     Fast,
//     // Use bitfields to track initialization state. Slower setting and lookup, less memory usage.
//     Dense
// };

} // namespace daedalus::containers

#endif
