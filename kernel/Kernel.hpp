#pragma once

#include <lazy_instance.hpp>

#include "Logger/Logger.hpp"
#include "SerialOutput.hpp"
#include "Interrupt/Manager.hpp"
#include "Process/Scheduler.hpp"

struct Kernel
{
    Kernel();
    void run();

    static process::Scheduler& getScheduler();
    static interrupt::Manager& getInterruptManager();

    static void NORETURN panic(const char* message);

private:
    static yacppl::lazy_instance<SerialOutput> serialOutput_;
    static yacppl::lazy_instance<process::Scheduler> scheduler_;
    static yacppl::lazy_instance<interrupt::Manager> interruptManager_;
    Logger logger_;
};
