#include "Logger/Logger.hpp"
#include "Cpu/Instructions.hpp"
#include "Memory/Allocator.hpp"

void* operator new(size_t size)
{
    return memory::heapAllocator->allocate(size);
}

void operator delete(void* address) noexcept
{
    memory::heapAllocator->free(address);
}

void operator delete(void* address, size_t) noexcept
{
    memory::heapAllocator->free(address);
}

void* operator new[](size_t size)
{
    return memory::heapAllocator->allocate(size);
}

void operator delete[](void* address) noexcept
{
    memory::heapAllocator->free(address);
}

void operator delete[](void* address, size_t) noexcept
{
    memory::heapAllocator->free(address);
}

asmlinkage
{

void* __dso_handle = nullptr;

int __cxa_atexit(void (*)(void*), void*, void*)
{
    return 0;
}

void __cxa_pure_virtual()
{
    Logger("cxx") << error << __func__ << ": PANIC";
    while (1)
    {
        cpu::halt();
    }
}

// for clang
void* memset(void* ptr, int value, unsigned n)
{
    for (auto i = 0u; i < n; ++i)
    {
        *reinterpret_cast<char*>(ptr) = value;
    }
    return ptr;
}

using InitFunction = void (*)();

asmlinkage InitFunction __init_array_start[];
asmlinkage InitFunction __init_array_end[];

asmlinkage InitFunction __preinit_array_start[];
asmlinkage InitFunction __preinit_array_end[];

void globalConstructors()
{
    for (auto initConstructor = __preinit_array_start; initConstructor != __preinit_array_end; ++initConstructor)
    {
        (*initConstructor)();
    }

    for (auto initConstructor = __init_array_start; initConstructor != __init_array_end; ++initConstructor)
    {
        (*initConstructor)();
    }
}

} // asmlinkage
