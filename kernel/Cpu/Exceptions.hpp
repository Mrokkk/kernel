#define __NR_divide_error           0
#define __NR_debug                  1
#define __NR_nmi                    2
#define __NR_breakpoint             3
#define __NR_overflow               4
#define __NR_bound_range            5
#define __NR_invalid_opcode         6
#define __NR_device_na              7
#define __NR_double_fault           8
#define __NR_coprocessor            9
#define __NR_invalid_tss            10
#define __NR_segment_np             11
#define __NR_stack_segment          12
#define __NR_general_protection     13
#define __NR_page_fault             14

#define __STRING_divide_error       "Division By Zero"
#define __STRING_debug              "Debug"
#define __STRING_nmi                "Non Maskable Interrupt"
#define __STRING_breakpoint         "Breakpoint"
#define __STRING_overflow           "Into Detected Overflow"
#define __STRING_bound_range        "Out of Bounds"
#define __STRING_invalid_opcode     "Invalid Opcode"
#define __STRING_device_na          "No Coprocessor"
#define __STRING_double_fault       "Double Fault"
#define __STRING_coprocessor        "Coprocessor Segment Overrun"
#define __STRING_invalid_tss        "Bad TSS"
#define __STRING_segment_np         "Segment Not Present"
#define __STRING_stack_segment      "Stack Fault"
#define __STRING_general_protection "General Protection Fault"
#define __STRING_page_fault         "Page Fault"

#ifndef EXCEPTION_ERRNO
#define EXCEPTION_ERRNO(...)
#endif

#ifndef EXCEPTION_NO_ERRNO
#define EXCEPTION_NO_ERRNO(...)
#endif

EXCEPTION_NO_ERRNO(divide_error)
EXCEPTION_NO_ERRNO(debug)
EXCEPTION_NO_ERRNO(nmi)
EXCEPTION_NO_ERRNO(breakpoint)
EXCEPTION_NO_ERRNO(overflow)
EXCEPTION_NO_ERRNO(bound_range)
EXCEPTION_NO_ERRNO(invalid_opcode)
EXCEPTION_NO_ERRNO(device_na)
EXCEPTION_ERRNO(double_fault)
EXCEPTION_NO_ERRNO(coprocessor)
EXCEPTION_ERRNO(invalid_tss)
EXCEPTION_ERRNO(segment_np)
EXCEPTION_ERRNO(stack_segment)
EXCEPTION_ERRNO(general_protection)
EXCEPTION_ERRNO(page_fault)
