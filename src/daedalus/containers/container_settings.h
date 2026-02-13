#ifndef DAEDALUS_CONTAINERS_CONTAINER_SETTINGS_H
#define DAEDALUS_CONTAINERS_CONTAINER_SETTINGS_H

#include <cstdint>

namespace daedalus::containers
{

enum class ManagementMode : uint8_t
{
    // Container should not manage anything for the user. Destructors will not be called.
    Unmanaged = 0,
    // Container will track what elements have been populated and call destructors on overwrite and on container
    // destructor.
    Managed = 1,
    // All checks of Managed mode but with added bounds checks.
    ManagedWithBoundsChecks = 2
};

} // namespace daedalus::containers

#endif
