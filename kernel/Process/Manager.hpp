#pragma once

#include "Process.hpp"
#include "kernel/Cpu/StackFrame.hpp"

namespace process
{

struct Manager
{
    Manager();

    Process::Pid createKernelProcess(const cpu::StackFrame& frame);
};

} // namespace process
