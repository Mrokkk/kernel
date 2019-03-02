ENTRY(_start)

__kernel_phys = 0x00100000;
__kernel_virt = 0xC0100000;

__kernel_offset = __kernel_virt - __kernel_phys;

SECTIONS
{
    . = __kernel_phys;

    .unpagged BLOCK(4K) : ALIGN(4K)
    {
        __unpagged_start = .;
        KEEP(*(.multiboot))
        KEEP(*(.boot))
        *Multiboot[1-2].cpp.o(.rodata)
        __unpagged_end = .;
    }

    . += __kernel_offset;

    . = ALIGN(4K);
    .text BLOCK(4K) : AT(ADDR(.text) - __kernel_offset)
    {
        __text_start = .;
        *(.text*)
        *(.gnu.linkonce.*)
        __text_end = .;
    }

    . = ALIGN(4K);
    .rodata BLOCK(4K) : AT(ADDR(.rodata) - __kernel_offset)
    {
        KEEP (*(.rodata*))
    }

    . = ALIGN(4);
    .preinit_array : AT(ADDR(.preinit_array) - __kernel_offset)
    {
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP (*(.preinit_array*))
        PROVIDE_HIDDEN (__preinit_array_end = .);
    }

    . = ALIGN(4);
    .init_array : AT(ADDR(.init_array) - __kernel_offset)
    {
        __init_array_start = .;
        KEEP (*(SORT_BY_INIT_PRIORITY(.init_array*) SORT_BY_INIT_PRIORITY(.ctors*)))
        __init_array_end = .;
    }

    . = ALIGN(4K);
    .data BLOCK(4K) : AT(ADDR(.data) - __kernel_offset)
    {
        __data_start = .;
        KEEP (*(.data))
        KEEP (*(.data.*))
        __data_end = .;
    }

    . = ALIGN(4K);
    .bss BLOCK(4K) : AT(ADDR(.bss) - __kernel_offset)
    {
        __bss_start = .;
        *(COMMON)
        *(.bss*)
        __bss_end = .;
    }

    . = ALIGN(4K);

    __heap_start = .;

    /DISCARD/ :
    {
        *(.dynamic)
        *(.gnu.hash)
        *(.dynstr)
        *(.dynsym)
        *(.eh_frame)
        *(.note)
        *(.comment)
    }
}
