#pragma once

#include "kernel/Cpu/StackFrame.hpp"

namespace interrupt
{

struct Irq final
{
    using Handler = void(*)(uint32_t, cpu::StackFrame*);

    Handler handler_;
    const char* name_;
};

} // namespace interrupt
