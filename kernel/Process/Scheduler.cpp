#include "Scheduler.hpp"

#include "kernel/Cpu/Gdt.hpp"
#include "kernel/Cpu/Tss.hpp"
#include "kernel/Cpu/IrqLock.hpp"
#include "kernel/Cpu/Segment.hpp"
#include "kernel/Cpu/Syscall.hpp"
#include "kernel/Cpu/Registers.hpp"
#include "kernel/Cpu/StackFrame.hpp"
#include "kernel/Cpu/Instructions.hpp"
#include "kernel/Memory/VirtualMemoryManager.hpp"

#include "kernel/Kernel.hpp"

#include "kernel/Memory/Memory.hpp"

asmlinkage uint32_t kernelStack[];
asmlinkage void returnFromSyscall();

namespace process
{

Scheduler::Scheduler()
    : initProcess_(
        reinterpret_cast<uint32_t>(kernelStack) - INIT_KERNEL_STACK,
        INIT_KERNEL_STACK)
    , runQueue_(&Process::running)
    , currentProcess_(&initProcess_)
    , nextPid_(1)
    , logger_(__func__)
{
    initialize();
}

void Scheduler::initialize()
{
    logger_ << info << __func__;

    cpu::gdt::setTss(initProcess_.context);
}

void Scheduler::schedule()
{
    auto lastContext = &currentProcess_->context;

    if (runQueue_.empty())
    {
        currentProcess_ = &initProcess_;
    }
    else
    {
        if (currentProcess_->state != Process::State::Running or currentProcess_ == &initProcess_)
        {
            currentProcess_ = &runQueue_.front();
        }
        else
        {
            auto nextNode = currentProcess_->running.next();
            if (runQueue_.end() == nextNode)
            {
                nextNode = nextNode->next();
            }
            currentProcess_ = nextNode->entry();
        }
    }

    if (lastContext == &currentProcess_->context)
    {
        return;
    }

    cpu::contextSwitch(*lastContext, currentProcess_->context);
}

uint32_t copyStack(
    const uint32_t destination,
    const uint32_t source,
    const uint32_t size)
{
    const auto destinationBegin = reinterpret_cast<char*>(destination - size);
    const auto sourceBegin = reinterpret_cast<char*>(source - size);

    yacppl::memcopy(sourceBegin, destinationBegin, size);

    return reinterpret_cast<uint32_t>(destination) - size;
}

Process::Pid Scheduler::createKernelProcess(const cpu::StackFrame& frame)
{
    auto lock = cpu::IrqLock::make();

    auto& currentProcess = getCurrentProcess();

    auto newProcess = yacppl::make_shared<Process>(
        currentProcess,
        nextPid_++,
        Process::Type::Kernel,
        PAGE_SIZE);

    logger_ << info << __func__ << ": created " << *newProcess;

    auto stack = copyStack(
        newProcess->kernelStack.end(),
        currentProcess.kernelStack.end(),
        currentProcess.kernelStack.end() - reinterpret_cast<uint32_t>(&frame.esp));

    stack = cpu::StackFrame::clone(frame, stack, stack);

    newProcess->context.eip = reinterpret_cast<uint32_t>(&returnFromSyscall);
    newProcess->context.esp = reinterpret_cast<uint32_t>(stack);

    newProcess->state = Process::State::Running;

    processes_.push_back(newProcess);
    runQueue_.push_back(*newProcess);

    return newProcess->pid;
}

Process::Pid Scheduler::createUserProcess(const cpu::StackFrame& frame)
{
    auto lock = cpu::IrqLock::make();

    auto& currentProcess = getCurrentProcess();

    const constexpr auto stackSize = 2048u;

    auto newProcess = yacppl::make_shared<Process>(
        currentProcess,
        nextPid_++,
        Process::Type::User,
        stackSize);

    auto stack = copyStack(
        newProcess->kernelStack.end(),
        currentProcess.kernelStack.end(),
        currentProcess.kernelStack.end() - reinterpret_cast<uint32_t>(frame.esp));

    stack = cpu::StackFrame::clone(frame, stack, stack);

    newProcess->context.eip = reinterpret_cast<uint32_t>(&returnFromSyscall);
    newProcess->context.esp = reinterpret_cast<uint32_t>(stack);

    newProcess->state = Process::State::Running;

    processes_.push_back(newProcess);
    runQueue_.push_back(*newProcess);

    return newProcess->pid;
}

Process& Scheduler::getCurrentProcess()
{
    return *currentProcess_;
}

void NORETURN Scheduler::exit(const int exitCode)
{
    auto lock = cpu::IrqLock::make();

    currentProcess_->exitCode = exitCode;
    currentProcess_->state = Process::State::Zombie;
    runQueue_.erase(*currentProcess_);

    logger_ << info << __func__ << ": exitted " << *currentProcess_;

    schedule();

    Kernel::panic("reached unreachable");
}

asmlinkage void do_scheduler()
{
    Kernel::getScheduler().schedule();
}

asmlinkage int sys_clone(cpu::StackFrame frame)
{
    return Kernel::getScheduler().createKernelProcess(frame);
}

asmlinkage int sys_getpid(cpu::StackFrame)
{
    return Kernel::getScheduler().getCurrentProcess().pid;
}

asmlinkage int sys_getppid(cpu::StackFrame)
{
    return Kernel::getScheduler().getCurrentProcess().ppid;
}

asmlinkage int sys_exit(cpu::StackFrame frame)
{
    auto exitCode = syscall::getSyscallParam<int>(frame, 1);
    Kernel::getScheduler().exit(exitCode);
    return 0;
}

} // namespace process
