#pragma once

namespace cpu
{
namespace io
{

inline uint8_t inb(const uint16_t port)
{
    uint8_t rv;
    asm volatile("inb %1, %0" : "=a" (rv) : "dN" (port));
    return rv;
}

inline void outb(const uint8_t data, const uint16_t port)
{
    asm volatile("outb %1, %0" : : "dN" (port), "a" (data));
}

inline void ioWait()
{
    asm volatile(R"(
        jmp 1f
        1: jmp 2f
        2:
    )");
}

} // namespace io
} // namespace cpu
