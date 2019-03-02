#include "Kernel.hpp"

#include <shared_ptr.hpp>

#include "Boot/Boot.hpp"

#include "Cpu/Gdt.hpp"
#include "Cpu/Idt.hpp"
#include "Cpu/Pic.hpp"
#include "Cpu/Pit.hpp"
#include "Cpu/Reboot.hpp"
#include "Cpu/Syscall.hpp"
#include "Cpu/Instructions.hpp"

#include "Memory/Memory.hpp"
#include "Memory/Sections.hpp"

volatile size_t jiffies;

yacppl::lazy_instance<SerialOutput> Kernel::serialOutput_;
yacppl::lazy_instance<process::Scheduler> Kernel::scheduler_;
yacppl::lazy_instance<interrupt::Manager> Kernel::interruptManager_;

Kernel::Kernel()
    : logger_(__func__)
{
}

void process1()
{
    auto i = 0u;
    while (1)
    {
        ++i;
        if (i == 200)
        {
            exit(2);
        }
        cpu::halt();
    }
}

void process3()
{
    Logger(__func__) << info << getpid();
    auto i = 0u;
    while (1)
    {
        ++i;
        if (i == 100)
        {
            exit(-1);
        }
        cpu::halt();
    }
}

void process2()
{
    Logger(__func__) << info << getpid();

    auto i = 0u;
    while (1)
    {
        ++i;
        if (clone() == 0)
        {
            process3();
        }
        if (i == 50)
        {
            exit(2);
        }
    }
}

NORETURN void Kernel::run()
{
    logger_ << info << __func__;

    cpu::gdt::initialize();
    cpu::idt::initialize();
    memory::initialize();

    cpu::pic::initialize();
    cpu::pit::initialize();
    syscall::initialize();

    scheduler_.initialize();
    interruptManager_.initialize();

    serialOutput_.initialize(SerialOutput::Port::Com1);

    logger_.initialize(serialOutput_.get());

    boot::printBootInfo();

    cpu::sti();

    if (clone() == 0)
    {
        process1();
    }

    if (clone() == 0)
    {
        process2();
    }

    logger_ << info << __func__ << ": entering idle";

    while (1)
    {
        cpu::halt();
    }
}

process::Scheduler& Kernel::getScheduler()
{
    return *scheduler_.get();
}

interrupt::Manager& Kernel::getInterruptManager()
{
    return *interruptManager_.get();
}

void NORETURN Kernel::panic(const char* message)
{
    Logger("Kernel") << error << __func__ << ": " << message;
    while (1)
    {
        cpu::halt();
    }
}
