#ifndef DAEDALUS_MATH_MATH_H
#define DAEDALUS_MATH_MATH_H

namespace dae
{
static auto align_up(size_t val, size_t alignment) -> size_t
{
    return ((val + alignment - 1) / alignment) * alignment;
}
} // namespace dae

#endif
