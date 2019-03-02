#pragma once

#include "Irq.hpp"
#include "kernel/Logger/Logger.hpp"

namespace interrupt
{

struct Manager
{
    Manager();
    void initialize();
    void registerHandler(uint32_t nr, Irq::Handler fn, const char* name);
    void handleIrq(uint32_t nr, cpu::StackFrame* frame);

private:
    Irq irqs_[32];
    Logger logger_;
};

} // namespace interrupt
