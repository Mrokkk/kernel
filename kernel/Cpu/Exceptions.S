#include "Segment.hpp"

#define REGS_EBX    0
#define REGS_ECX    4
#define REGS_EDX    8
#define REGS_ESI    12
#define REGS_EDI    16
#define REGS_EBP    20
#define REGS_EAX    24
#define REGS_DS     28
#define REGS_ES     32
#define REGS_FS     36
#define REGS_GS     40
#define REGS_EIP    44
#define REGS_CS     48
#define REGS_EFLAGS 52
#define REGS_ESP    56
#define REGS_SS 60

#define SAVE_ALL \
    pushl %gs;          /* gs */  \
    pushl %fs;          /* fs */  \
    pushl %es;          /* es */  \
    pushl %ds;          /* ds */  \
    push %eax;          /* eax */ \
    mov $KERNEL_DS, %eax;         \
    mov %ax, %ds;                 \
    mov %ax, %es;                 \
    mov %ax, %fs;                 \
    mov (%esp), %eax;             \
    push %ebp;          /* ebp */ \
    push %edi;          /* edi */ \
    push %esi;          /* esi */ \
    push %edx;          /* edx */ \
    push %ecx;          /* ecx */ \
    push %ebx;          /* ebx */

#define RESTORE_ALL \
    pop %ebx; \
    pop %ecx; \
    pop %edx; \
    pop %esi; \
    pop %edi; \
    pop %ebp; \
    pop %eax; \
    popl %ds; \
    popl %es; \
    popl %fs; \
    popl %gs;

#define PIC_EOI(nr) \
    mov $0x20, %al; \
    .if nr > 7; \
        outb %al, $0xa0; \
    .endif; \
    outb %al, $0x20

#define PIC_ISR(x) \
    ENTRY(isr_##x) \
        SAVE_ALL; \
        push %esp; \
        push $x-0x20; \
        call do_irq; \
        PIC_EOI(x); \
        add $8, %esp; \
        RESTORE_ALL; \
        iret;

#define EXCEPTION_NO_ERRNO(x) \
    ENTRY(exc_##x##_handler) \
        SAVE_ALL; \
        push $0; \
        push $__NR_##x; \
        call exceptionHandler; \
        add $12, %esp; \
        RESTORE_ALL; \
        iret;

#define EXCEPTION_ERRNO(x) \
    ENTRY(exc_##x##_handler) \
        mov (%esp), %eax; \
        add $4, %esp; \
        SAVE_ALL; \
        push %eax; \
        push $__NR_##x; \
        call exceptionHandler; \
        add $12, %esp; \
        RESTORE_ALL; \
        iret;

#define __syscall0(name) \
    ENTRY(name) \
        mov $__NR_##name, %eax; \
        int $0x80; \
        ret;

#define __syscall1(name) \
    ENTRY(name) \
        mov $__NR_##name, %eax; \
        mov 4(%esp), %ebx; \
        int $0x80; \
        ret;

#define __syscall2(name) \
    ENTRY(name) \
        push %ebx; \
        mov $__NR_##name, %eax; \
        mov 8(%esp), %ebx; \
        mov 12(%esp), %ecx; \
        int $0x80; \
        pop %ebx; \
        ret;

#define __syscall3(name) \
    ENTRY(name) \
        push %ebx; \
        mov $__NR_##name, %eax; \
        mov 8(%esp), %ebx; \
        mov 12(%esp), %ecx; \
        mov 16(%esp), %edx; \
        int $0x80; \
        pop %ebx; \
        ret;

#define __syscall4(name) \
    ENTRY(name) \
        push %ebx; \
        push %esi; \
        mov $__NR_##name, %eax; \
        mov 12(%esp), %ebx; \
        mov 16(%esp), %ecx; \
        mov 20(%esp), %edx; \
        mov 24(%esp), %esi; \
        int $0x80; \
        pop %esi; \
        pop %ebx; \
        ret;

#define __syscall5(name) \
    ENTRY(name) \
        push %ebx; \
        push %esi; \
        push %edi; \
        mov $__NR_##name, %eax; \
        mov 16(%esp), %ebx; \
        mov 20(%esp), %ecx; \
        mov 24(%esp), %edx; \
        mov 28(%esp), %esi; \
        mov 32(%esp), %edi; \
        int $0x80; \
        pop %edi; \
        pop %esi; \
        pop %ebx; \
        ret;

#include "Irqs.hpp"
#include "Syscall.hpp"
#include "Exceptions.hpp"

ENTRY(systickHandler)
    incl jiffies
    SAVE_ALL
    PIC_EOI(0)
    call do_scheduler
    RESTORE_ALL
    iret

ENTRY(syscallHandler)
    SAVE_ALL
    cmp $__NR_syscalls, %eax
    jg badSyscall
    mov syscalls(, %eax, 4), %ebx
    cmp $0, %ebx
    jz badSyscall
    call *%ebx
    mov %eax, REGS_EAX(%esp)

ENTRY(returnFromSyscall)
    RESTORE_ALL
    iret

badSyscall:
    movl $-1, REGS_EAX(%esp)
    jmp returnFromSyscall
