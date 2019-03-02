#include "kernel/Memory/Memory.hpp"
#include "kernel/Boot/Multiboot.hpp"

ALIGN(4)

.section .multiboot,"ax",@progbits

MULTIBOOT_HEADER()

.section .boot,"ax",@progbits

ENTRY(_start)
    // Set stack pointer and reset flags
    mov $kernelStack - KERNEL_PAGE_OFFSET, %esp
    pushl $0
    popfl
    push %eax
    push %ebx

    // Zero BSS section
    movl $__bss_start - KERNEL_PAGE_OFFSET, %edi
    movl $__bss_end - KERNEL_PAGE_OFFSET, %ecx
    subl %edi, %ecx
    cld
    rep

    call readBootloaderData

    mov $page0, %edi
    movl $PAGE_INIT_FLAGS, %eax
1:  stosl
    add $PAGE_SIZE, %eax
    cmp $0x100000 + PAGE_INIT_FLAGS, %eax
    jne 1b
1:  stosl
    add $PAGE_SIZE, %eax
    cmp $__heap_start - KERNEL_PAGE_OFFSET + PAGE_INIT_FLAGS, %eax
    jne 1b

    mov $pageDir, %eax
    movl %eax, %cr3

    mov %cr0, %eax
    or $0x80000000, %eax
    mov %eax, %cr0
    mov $1f, %eax
    jmp *%eax

1:  mov $kernelStack, %esp
    call globalConstructors
    call main

1:  jmp 1b

ALIGN(PAGE_SIZE)
ENTRY(pageDir)
    .long pageTable0
    .fill KERNEL_PAGE_OFFSET / 0x400000 - 1, 4, 0
    .long pageTable0

ALIGN(PAGE_SIZE)
ENTRY(page0)
    .skip PAGE_INIT_FLAGS
pageTable0:
    .skip PAGE_SIZE - PAGE_INIT_FLAGS

SECTION(.bss)

.skip INIT_KERNEL_STACK
ENTRY(kernelStack)
