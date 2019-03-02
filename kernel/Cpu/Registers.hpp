#pragma once

namespace cpu
{
namespace registers
{

struct Cr0
{
    static inline uint32_t get()
    {
        uint32_t value;
        asm volatile("mov %%cr0, %0" : "=r" (value));
        return value;
    }
};

struct Cr1
{
    static inline uint32_t get()
    {
        uint32_t value;
        asm volatile("mov %%cr1, %0" : "=r" (value));
        return value;
    }
};

struct Cr2
{
    static inline uint32_t get()
    {
        uint32_t value;
        asm volatile("mov %%cr2, %0" : "=r" (value));
        return value;
    }
};

struct Cr3
{
    static inline uint32_t get()
    {
        uint32_t value;
        asm volatile("mov %%cr3, %0" : "=r" (value));
        return value;
    }

    static inline void set(const uint32_t value)
    {
        asm volatile("mov %0, %%cr3" :: "r" (value));
    }
};

struct Ebp
{
    static inline uint32_t get()
    {
        uint32_t value;
        asm volatile("mov %%ebp, %0" : "=r" (value));
        return value;
    }
};

struct Esp
{
    static inline uint32_t get()
    {
        uint32_t value;
        asm volatile("mov %%esp, %0" : "=r" (value));
        return value;
    }
};

struct Eflags
{
    static inline uint32_t get()
    {
        uint32_t value;
        asm volatile("pushfl; pop %0" : "=r" (value));
        return value;
    }
};

} // namespace registers
} // namespace cpu
