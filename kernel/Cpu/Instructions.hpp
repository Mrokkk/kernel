#pragma once

namespace cpu
{

inline void halt()
{
    asm volatile("hlt");
}

inline void cli()
{
    asm volatile("cli");
}

inline void sti()
{
    asm volatile("sti");
}

} // namespace cpu
