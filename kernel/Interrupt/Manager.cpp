#include "Manager.hpp"

#include "kernel/Kernel.hpp"

#include "kernel/Cpu/Io.hpp"
#include "kernel/Cpu/IrqLock.hpp"
#include "kernel/Cpu/StackFrame.hpp"

using namespace cpu::io;

namespace interrupt
{

namespace
{

constexpr const auto PIC1 = 0x20u;
constexpr const auto PIC2 = 0xA0u;

constexpr const auto ICW2_PIC1 = 0x20u;
constexpr const auto ICW2_PIC2 = 0x28u;

uint16_t mask = 0xffff;

void emptyIsr(uint32_t, cpu::StackFrame*) {}

void picDisable()
{
    auto lock = cpu::IrqLock::make();
    outb(0xff, PIC1 + 1);
    outb(0xff, PIC2 + 1);
}

inline void sendIcw1()
{
    outb(0x11, PIC1);
    outb(0x11, PIC2);
}

inline void sendIcw2()
{
    outb(ICW2_PIC1, PIC1 + 1);
    outb(ICW2_PIC2, PIC2 + 1);
}

inline void sendIcw3()
{
    outb(4, PIC1 + 1);
    outb(2, PIC2 + 1);
}

inline void sendIcw4()
{
    outb(1, PIC1 + 1);
    outb(1, PIC2 + 1);
}

void enable(const uint32_t irq)
{
    auto lock = cpu::IrqLock::make();

    mask &= ~(1 << irq);

    if (irq < 8)
    {
        outb(mask & 0xff, PIC1 + 1);
    }
    else
    {
        outb(mask >> 8, PIC2 + 1);
    }
}

} // namespace

Manager::Manager()
    : logger_(__func__)
{
    initialize();
}

void Manager::initialize()
{
    sendIcw1();
    sendIcw2();
    sendIcw3();
    sendIcw4();

    picDisable();

    registerHandler(0, &emptyIsr, "timer");
    registerHandler(2, &emptyIsr, "cascade");
    registerHandler(13, &emptyIsr, "fpu");
}

void Manager::registerHandler(uint32_t nr, Irq::Handler fn, const char* name)
{
    if (irqs_[nr].handler_)
    {
        logger_ << error << __func__ << ": cannot register handler for IRQ " << nr << "; already registered";
        return;
    }
    irqs_[nr].handler_ = fn;
    irqs_[nr].name_ = name;
    enable(nr);
    logger_ << info << __func__ << ": registered handler for IRQ " << nr << " " << name;
}

void Manager::handleIrq(uint32_t nr, cpu::StackFrame* frame)
{
    if (irqs_[nr].handler_)
    {
        irqs_[nr].handler_(nr, frame);
        return;
    }
    logger_ << error << __func__ << ": not handled interrupt " << (int)nr;
}

asmlinkage void do_irq(const uint32_t nr, cpu::StackFrame* stackFrame)
{
    Kernel::getInterruptManager().handleIrq(nr, stackFrame);
}

} // namespace interrupt
