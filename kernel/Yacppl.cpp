#include <spinlock.hpp>

namespace yacppl
{

void spinlock::lock()
{
    auto dummy = static_cast<size_t>(state::locked);
    asm volatile(R"(
        1: lock xchg %0, %1
        test %1, %1
        jnz 1b)"
        : "=m" (lock_)
        : "r" (dummy)
        : "memory");
}

void spinlock::unlock()
{
    auto dummy = static_cast<size_t>(state::unlocked);
    asm volatile(
        "lock xchg %0, %1"
        : "=m" (lock_)
        : "r" (dummy)
        : "memory");
}

} // namespace yacppl
