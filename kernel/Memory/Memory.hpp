#pragma once

#include "kernel/Cpu/Mmu.hpp"

#define KERNEL_PAGE_OFFSET 0xc0000000
#define INIT_KERNEL_STACK 8192

#ifndef __ASSEMBLER__

#include "kernel/Logger/Logger.hpp"

namespace memory
{

struct MemoryMap
{
    enum class Type
    {
        Unknown,
        Unavailable,
        Available,
        Device
    };

    Type type;
    uint32_t base;
    uint32_t size;
};

extern struct MemoryMap memoryMap[];

template <typename T>
constexpr inline T align(T address, const uint32_t alignment)
{
    const auto mod = reinterpret_cast<uint32_t>(address) % alignment;
    if (mod != 0)
    {
        return reinterpret_cast<T>(((reinterpret_cast<uint32_t>(address) + alignment - 1) / 1024) * 1024);
    }
    return address;
}

template <typename T>
constexpr inline bool inKernelSpace(T address)
{
    return reinterpret_cast<uint32_t>(address) > KERNEL_PAGE_OFFSET;
}

template <typename T>
constexpr inline T phys2virt(T address)
{
    return reinterpret_cast<T>(reinterpret_cast<uint32_t>(address) + KERNEL_PAGE_OFFSET);
}

template <typename T>
constexpr inline T virt2phys(T address)
{
    return reinterpret_cast<T>(reinterpret_cast<uint32_t>(address) - KERNEL_PAGE_OFFSET);
}

template <typename T>
constexpr inline uint32_t frameNumber(T address)
{
    return reinterpret_cast<uint32_t>(address) / PAGE_SIZE;
}

constexpr inline uint32_t frameAddress(size_t index)
{
    return index * PAGE_SIZE;
}

template <typename T>
constexpr inline uint32_t pageTableIndex(T address)
{
    return PAGE_TABLES_NUMBER * (reinterpret_cast<uint32_t>(address) / PAGE_SIZE) / (((uint32_t)0 - 1) / PAGE_SIZE);
}

void initialize();

LineWrapper& operator<<(LineWrapper& stream, MemoryMap::Type type);
LineWrapper& operator<<(LineWrapper& stream, const MemoryMap& type);

} // namespace memory

#endif
