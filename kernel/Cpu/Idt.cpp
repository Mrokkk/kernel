#include "Idt.hpp"

#include "kernel/Logger/Logger.hpp"

#include "Gdt.hpp"

namespace cpu
{
namespace idt
{

namespace
{

struct Entry final
{
    void setGate(
        const uint32_t base,
        const uint16_t selector,
        const uint32_t flags)
    {
        baseLo_ = base & 0xFFFF;
        baseHi_ = (base >> 16) & 0xFFFF;
        selector_ = selector;
        always0_ = 0;
        flags_ = flags | 0x80;
    }

private:
    uint16_t baseLo_;
    uint16_t selector_;
    uint8_t always0_;
    uint8_t flags_;
    uint16_t baseHi_;
} PACKED;

struct Idtr final
{
    void load()
    {
        asm volatile(
            "lidt (%%eax);"
            :: "a" (this));
    }

    uint16_t limit;
    Entry* base;
} PACKED;

Entry entries[256];

Idtr idtr = {
    sizeof(entries) - 1,
    entries
};

} // namespace

asmlinkage void systickHandler();
asmlinkage void syscallHandler();

#define PIC_ISR(x) \
    asmlinkage void isr_##x();

#define EXCEPTION_ERRNO(x) \
    asmlinkage void exc_##x##_handler();

#define EXCEPTION_NO_ERRNO(x) \
    asmlinkage void exc_##x##_handler();

#include "Irqs.hpp"
#include "Exceptions.hpp"

#undef TIMER_ISR
#undef PIC_ISR
#undef SYSCALL_ISR
#undef EXCEPTION_ERRNO
#undef EXCEPTION_NO_ERRNO

#define TIMER_ISR(x) \
    entries[x].setGate(reinterpret_cast<uint32_t>(systickHandler), Segment::KernelCs, gdt::flags::Type::TrapGate32);

#define PIC_ISR(x) \
    entries[x].setGate(reinterpret_cast<uint32_t>(isr_##x), Segment::KernelCs, gdt::flags::Type::TrapGate32);

#define SYSCALL_ISR(x) \
    Logger("Idt") << info << "configuring syscall"; \
    entries[x].setGate(reinterpret_cast<uint32_t>(syscallHandler), Segment::KernelCs, gdt::flags::Type::TrapGate32 | gdt::flags::Ring3);

#define EXCEPTION_ERRNO(x) \
    entries[__NR_##x].setGate(reinterpret_cast<uint32_t>(exc_##x##_handler), Segment::KernelCs, gdt::flags::Type::TrapGate32);

#define EXCEPTION_NO_ERRNO(x) \
    entries[__NR_##x].setGate(reinterpret_cast<uint32_t>(exc_##x##_handler), Segment::KernelCs, gdt::flags::Type::TrapGate32);

void initialize()
{
    Logger("Idt") << info << __func__;

    #include "Irqs.hpp"
    #include "Exceptions.hpp"

    idtr.load();
}

} // namespace idt
} // namespace cpu
