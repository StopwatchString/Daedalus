#ifndef DAEDALUS_MATH_H
#define DAEDALUS_MATH_H

namespace daedalus::math
{
static auto align_up(size_t val, size_t alignment) -> size_t
{
    return ((val + alignment - 1) / alignment) * alignment;
}
} // namespace daedalus::math

#endif
