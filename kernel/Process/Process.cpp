#include "Process.hpp"

#include <shared_ptr.hpp>
#include <kernel_list.hpp>

#include "kernel/Logger/Logger.hpp"
#include "kernel/Cpu/Gdt.hpp"

namespace process
{

Process::Process(
    const uint32_t stackStart,
    const uint32_t stackSize)
    : pid(0)
    , ppid(-1)
    , type(Type::Kernel)
    , state(State::Stopped)
    , context()
    , kernelStack(stackStart, stackSize)
    , running()
{
}

Process::Process(
    Process& parent,
    const int pid,
    const Type type,
    const size_t stackSize)
    : pid(pid)
    , ppid(parent.pid)
    , type(type)
    , state(State::Stopped)
    , context()
    , kernelStack(memory::Stack::make(stackSize))
    , running()
{
}

Process::~Process() = default;

LineWrapper& operator<<(LineWrapper& stream, const Process& process)
{
    stream << "Process{this=" << &process << ", "
           << "pid=" << process.pid << ", "
           << "ppid=" << process.ppid << ", "
           << "state=" << static_cast<int>(process.state);

    if (process.state == Process::State::Zombie)
    {
        stream << ", exitCode=" << process.exitCode;
    }

    return stream << "}";
}

} // namespace process
