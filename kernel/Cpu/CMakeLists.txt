set_property(SOURCE Exceptions.S PROPERTY LANGUAGE C)
add_library(cpu OBJECT
    Pit.cpp
    Gdt.cpp
    Idt.cpp
    Tss.cpp
    Syscall.cpp
    Exceptions.S
    Exceptions.cpp
    StackFrame.cpp
)
