#pragma once

#include "kernel/Memory/Memory.hpp"

#include "Gdt.hpp"
#include "IrqLock.hpp"
#include "Instructions.hpp"

namespace cpu
{

inline void reboot()
{
    auto lock = IrqLock::make();

    auto gdt = reinterpret_cast<cpu::gdt::GdtRegister*>(memory::phys2virt(0u));
    gdt->load();

    while (1)
    {
        cpu::halt();
    }
}

} // namespace cpu
