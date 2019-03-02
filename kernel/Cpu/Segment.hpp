#pragma once

#define KERNEL_CS 0x08
#define KERNEL_DS 0x10
#define USER_CS 0x1b
#define USER_DS 0x23
#define FIRST_TSS 0x28

#ifndef __ASSEMBLER__

namespace cpu
{

enum Segment
{
    KernelCs = KERNEL_CS,
    KernelDs = KERNEL_DS,
    UserCs = USER_CS,
    UserDs = USER_DS,
    Task = FIRST_TSS
};

} // namespace cpu

#endif
