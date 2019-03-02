add_subdirectory(Cpu)
add_subdirectory(Boot)
add_subdirectory(Memory)
add_subdirectory(Logger)
add_subdirectory(Process)
add_subdirectory(Interrupt)

set_property(SOURCE Head.S PROPERTY LANGUAGE C)

add_library(kernel
    Head.S
    Cxx.cpp
    Kernel.cpp
    Yacppl.cpp
    SerialOutput.cpp
    $<TARGET_OBJECTS:cpu>
    $<TARGET_OBJECTS:boot>
    $<TARGET_OBJECTS:logger>
    $<TARGET_OBJECTS:memory>
    $<TARGET_OBJECTS:process>
    $<TARGET_OBJECTS:interrupt>
)
