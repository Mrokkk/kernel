#include "StackFrame.hpp"
#include "Segment.hpp"

namespace cpu
{

uint32_t StackFrame::clone(
    const cpu::StackFrame& oldFrame,
    uint32_t kernelStack,
    uint32_t userStack)
{
    auto offset = oldFrame.cs == cpu::Segment::UserCs
        ? sizeof(cpu::StackFrame)
        : sizeof(cpu::StackFrame) - 2 * sizeof(uint32_t);

    auto newFrame = reinterpret_cast<cpu::StackFrame*>(kernelStack - offset);

    *newFrame = oldFrame;

    newFrame->eax = 0;
    newFrame->eflags = oldFrame.eflags | 0x200u;
    newFrame->ebp = userStack + oldFrame.ebp - reinterpret_cast<uint32_t>(&oldFrame.esp);

    if (oldFrame.cs == cpu::Segment::UserCs)
    {
        newFrame->esp = userStack;
        newFrame->ss = cpu::Segment::UserCs;
    }

    return kernelStack - offset;
}

} // namespace cpu
