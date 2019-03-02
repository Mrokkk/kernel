#include "Manager.hpp"

#include "kernel/Kernel.hpp"
#include "kernel/Cpu/Pic.hpp"
#include "kernel/Cpu/StackFrame.hpp"

namespace interrupt
{

Manager::Manager()
    : logger_(__func__)
{
    initialize();
}

namespace
{
void emptyIsr(uint32_t, cpu::StackFrame*) {}
} // namespace

void Manager::initialize()
{
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
    cpu::pic::enable(nr);
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
