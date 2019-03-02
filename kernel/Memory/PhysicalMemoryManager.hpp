#pragma once

#include <bitset.hpp>
#include "kernel/Logger/Logger.hpp"
#include "Memory.hpp"

namespace memory
{

struct PhysicalMemoryManager
{
    struct Frame
    {
        size_t index;
        uint32_t address;
    };

    PhysicalMemoryManager(
        const uint32_t availableMemory,
        const uint32_t memoryToCover);

    Frame allocateFrame();
    void deallocateFrame(const uint32_t address);
    uint32_t end() const;

private:
    const uint32_t end_;
    const size_t size_;
    yacppl::bitset<32768> data_;
    Logger log_;
};

} // namespace memory
