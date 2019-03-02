#pragma once

namespace cpu
{

struct IrqLock final
{
    IrqLock()
    {
        asm volatile("pushfl; popl %0; cli" : "=r" (flags_) :: "memory");
    }

    ~IrqLock()
    {
        asm volatile("pushl %0; popfl" :: "r" (flags_) : "memory");
    }

    static inline IrqLock make()
    {
        return IrqLock();
    }

private:
    uint32_t flags_;
};

} // namespace cpu
