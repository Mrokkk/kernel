#include "Tss.hpp"

#include "Gdt.hpp"

namespace cpu
{

Tss::Tss()
    : ss0(Segment::KernelDs)
    , iomapOffset(104)
    , ioBitmap()
{
}

void Tss::load() const
{
    asm volatile("ltr %%ax" :: "a" (Segment::Task));
}

asmlinkage void FASTCALL(switchTss(Tss*, Tss* next))
{
    gdt::setTss(*next);
}

void contextSwitch(Tss& prev, Tss& next)
{
    asm volatile(R"(
        push %%gs
        pushl %%ebx
        pushl %%ecx
        pushl %%esi
        pushl %%edi
        pushl %%ebp
        movl %%esp, %0
        movl %2, %%esp
        movl $1f, %1
        pushl %3
        jmp switchTss
        1:
        popl %%ebp
        popl %%edi
        popl %%esi
        popl %%ecx
        popl %%ebx
        pop %%gs)"
        : "=m" (prev.esp),
          "=m" (prev.eip)
        : "m" (next.esp),
          "m" (next.eip),
          "a" (&prev),
          "d" (&next));
}

} // namespace cpu
