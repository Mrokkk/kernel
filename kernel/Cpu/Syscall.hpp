#ifndef __SYSCALLS_H_
#define __SYSCALLS_H_

#define __NR_test 0

// Process management
#define __NR_exit 1
#define __NR_clone 2
#define __NR_waitpid 3
#define __NR_exec 4
#define __NR_getpid 5
#define __NR_getppid 6
#define __NR_nice 7
#define __NR_pause 8
#define __NR_kill 9
#define __NR_setgid 10
#define __NR_getgid 11
#define __NR_setuid 12
#define __NR_getuid 13

#define __NR_read 3
#define __NR_write 4
#define __NR_open 5
#define __NR_close 6
#define __NR_creat 8
#define __NR_link 9
#define __NR_unlink 10
#define __NR_chdir 12
#define __NR_time 13
#define __NR_mknod 14
#define __NR_chmod 15
#define __NR_lchown 16
#define __NR_stat 18
#define __NR_lseek 19
#define __NR_mount 21
#define __NR_umount 22
#define __NR_stime 25
#define __NR_ptrace 26
#define __NR_alarm 27
#define __NR_fstat 28
#define __NR_utime 30
#define __NR_stty 31
#define __NR_gtty 32

#define __NR_sleep 35
#define __NR_sync 36
#define __NR_switch 38

#define __NR_dup 41
#define __NR_pipe 42
#define __NR_times 43
#define __NR_prof 44

#define __NR_signal 48
#define __NR_readdir 49
#define __NR_sigreturn 119

#define __NR_syscalls 120

#ifndef __ASSEMBLER__

asmlinkage int test(int, int, int, int, int);
asmlinkage void exit(int);
asmlinkage int clone();
asmlinkage int getpid();

#include "StackFrame.hpp"

namespace syscall
{
void initialize();

template <typename T>
T getSyscallParam(cpu::StackFrame& frame, const size_t number)
{
    return static_cast<T>(*(reinterpret_cast<uint32_t*>(&frame) + (number - 1)));
}

} // namespace syscall

#endif /* __ASSEMBLER__ */

#endif // __SYSCALLS_H_

#ifndef __syscall0
#define __syscall0(call)
#endif

#ifndef __syscall1
#define __syscall1(call)
#endif

#ifndef __syscall2
#define __syscall2(call)
#endif

#ifndef __syscall3
#define __syscall3(call)
#endif

#ifndef __syscall4
#define __syscall4(call)
#endif

#ifndef __syscall5
#define __syscall5(call)
#endif

__syscall5(test)
__syscall1(exit)
__syscall0(clone)
__syscall0(getpid)
__syscall0(getppid)
