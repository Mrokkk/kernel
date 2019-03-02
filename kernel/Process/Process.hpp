#pragma once

#include <kernel_list.hpp>

#include "kernel/Cpu/Tss.hpp"
#include "kernel/Memory/Stack.hpp"
#include "kernel/Logger/Logger.hpp"
#include "kernel/Memory/VirtualMemoryManager.hpp"

namespace process
{

struct Process final
{
    using Pid = int;

    enum class State
    {
        Running,
        Waiting,
        Stopped,
        Zombie
    };

    enum class Type
    {
        Kernel,
        User
    };

    Process(
        const uint32_t stackStart,
        const uint32_t stackEnd);

    Process(
        Process& parent,
        const int pid,
        const Type type,
        const size_t stackSize);

    ~Process();

    const Pid pid;
    const Pid ppid;
    const Type type;
    State state;

    cpu::Tss context;
    memory::Stack kernelStack;
    memory::AddressSpace addressSpace;

    size_t contextSwitches;
    int exitCode;

    yacppl::kernel_list<Process>::node running;
};

LineWrapper& operator<<(LineWrapper& stream, const Process& process);

} // namespace process
