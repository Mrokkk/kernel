#pragma once

#include <list.hpp>
#include <shared_ptr.hpp>
#include <kernel_list.hpp>

#include "kernel/Logger/Logger.hpp"
#include "kernel/Cpu/StackFrame.hpp"

#include "Process.hpp"

namespace process
{

struct Scheduler
{
    Scheduler();
    void schedule();
    Process::Pid createKernelProcess(const cpu::StackFrame& frame);
    Process::Pid createUserProcess(const cpu::StackFrame& frame);
    Process& getCurrentProcess();
    void NORETURN exit(const int exitCode);

private:
    void initialize();

    Process initProcess_;
    yacppl::list<yacppl::shared_ptr<Process>> processes_;
    yacppl::kernel_list<Process> runQueue_;
    Process* currentProcess_;
    Process::Pid nextPid_;
    Logger logger_;
};

} // namespace process
