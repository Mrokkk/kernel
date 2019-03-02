#include "kernel/Kernel.hpp"
#include "kernel/Logger/Logger.hpp"

#include "StackFrame.hpp"

#define __syscall0(call) asmlinkage int sys_##call(cpu::StackFrame frame);
#define __syscall1(call) asmlinkage int sys_##call(cpu::StackFrame frame);
#define __syscall2(call) asmlinkage int sys_##call(cpu::StackFrame frame);
#define __syscall3(call) asmlinkage int sys_##call(cpu::StackFrame frame);
#define __syscall4(call) asmlinkage int sys_##call(cpu::StackFrame frame);
#define __syscall5(call) asmlinkage int sys_##call(cpu::StackFrame frame);
#include "Syscall.hpp"

asmlinkage int sys_test(cpu::StackFrame frame)
{
    Logger("Syscall::Test") << info
        << "param1: " << frame.ebx << ", "
        << "param2: " << frame.ecx << ", "
        << "param3: " << frame.edx << ", "
        << "param4: " << frame.esi << ", "
        << "param5: " << frame.edi;

    return 0;
}

#undef __syscall0
#undef __syscall1
#undef __syscall2
#undef __syscall3
#undef __syscall4
#undef __syscall5
#define __syscall0(call) syscalls[__NR_##call] = reinterpret_cast<uint32_t>(&sys_##call);
#define __syscall1(call) syscalls[__NR_##call] = reinterpret_cast<uint32_t>(&sys_##call);
#define __syscall2(call) syscalls[__NR_##call] = reinterpret_cast<uint32_t>(&sys_##call);
#define __syscall3(call) syscalls[__NR_##call] = reinterpret_cast<uint32_t>(&sys_##call);
#define __syscall4(call) syscalls[__NR_##call] = reinterpret_cast<uint32_t>(&sys_##call);
#define __syscall5(call) syscalls[__NR_##call] = reinterpret_cast<uint32_t>(&sys_##call);

uint32_t syscalls[__NR_syscalls];

namespace syscall
{

void initialize()
{
#include "Syscall.hpp"
}

} // namespace syscall
