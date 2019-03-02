#pragma once

#include <algorithm.hpp>
#include <iterator.hpp>
#include <kernel_list.hpp>
#include <lazy_instance.hpp>

#include "kernel/Logger/Logger.hpp"

#include "Memory.hpp"
#include "PhysicalMemoryManager.hpp"

namespace memory
{

struct AddressSpace
{
    cpu::mmu::PageDirectoryEntry* pageDir;
    yacppl::kernel_list<AddressSpace>::node node;
};

struct VirtualMemoryManager
{
    explicit VirtualMemoryManager(PhysicalMemoryManager* pmm);

    void unmap(cpu::mmu::PageDirectoryEntry* pageDir, void* address);
    cpu::mmu::PageDirectoryEntry* createAddressSpace();
    void* allocate();

private:
    void initialize();

    template <typename T>
    void clear(T table)
    {
        yacppl::fill((uint32_t*)table, (uint32_t*)table + 1024, 0);
    }

    inline void setPage(cpu::mmu::PageTableEntry* pageTable, int nr, uint32_t val)
    {
        reinterpret_cast<uint32_t*>(pageTable)[nr] = val;
    }

    inline void clearPage(cpu::mmu::PageTableEntry* pageTable, int nr)
    {
        reinterpret_cast<uint32_t*>(pageTable)[nr] = 0;
    }

    inline void setPageTable(cpu::mmu::PageDirectoryEntry* pageDir, int nr, uint32_t val)
    {
        reinterpret_cast<uint32_t*>(pageDir)[nr] = val;
    }

    inline void clearPageTable(cpu::mmu::PageDirectoryEntry* pageDir, int nr)
    {
        reinterpret_cast<uint32_t*>(pageDir)[nr] = 0;
    }

    PhysicalMemoryManager* pmm_;
    AddressSpace kernelAddressSpace_;
    yacppl::kernel_list<AddressSpace> addressSpaces_;
    Logger log_;
};

extern yacppl::lazy_instance<VirtualMemoryManager> vmm;

} // namespace memory
