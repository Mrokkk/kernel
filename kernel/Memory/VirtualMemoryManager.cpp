#include "VirtualMemoryManager.hpp"

#include "Memory.hpp"
#include "PhysicalMemoryManager.hpp"

using namespace cpu::mmu;

asmlinkage PageDirectoryEntry pageDir[];
asmlinkage PageTableEntry page0[];

namespace memory
{

PageDirectoryEntry kernelPageDir[1024] ALIGN(PAGE_SIZE);
PageTableEntry kernelPageTable[1024] ALIGN(PAGE_SIZE);

VirtualMemoryManager::VirtualMemoryManager(PhysicalMemoryManager* pmm)
    : pmm_(pmm)
    , addressSpaces_(&AddressSpace::node)
    , log_(__func__)
{
    initialize();
}

void VirtualMemoryManager::unmap(
    PageDirectoryEntry* pageDir,
    void* address)
{
    const auto tableIndex = reinterpret_cast<uint32_t>(address) >> 22;
    const auto pageIndex = (reinterpret_cast<uint32_t>(address) >> 12) & 0x03FF;

    auto pageTable = phys2virt(static_cast<uint32_t>(pageDir[tableIndex].address) << 12);

    clearPage(reinterpret_cast<PageTableEntry*>(pageTable), pageIndex);
    invlpg(address);
}

PageDirectoryEntry* VirtualMemoryManager::createAddressSpace()
{
    auto pageDir = allocate();

    yacppl::copy(
        reinterpret_cast<uint32_t*>(kernelPageDir),
        reinterpret_cast<uint32_t*>(pageDir),
        1024);

    log_ << info << __func__ << ": created address space with page directory @ " << pageDir;

    return static_cast<PageDirectoryEntry*>(pageDir);
}

void* VirtualMemoryManager::allocate()
{
    auto frame = pmm_->allocateFrame();
    setPage(kernelPageTable, frame.index, frame.address | PGT_PRESENT | PGT_WRITEABLE);

    log_ << info << __func__ << ": allocated " << frame.address;

    return reinterpret_cast<void*>(phys2virt(frame.address));
}

void VirtualMemoryManager::initialize()
{
    clear(kernelPageDir);
    clear(kernelPageTable);

    const auto heapStart = pmm_->end();
    const auto neededPages = heapStart / PAGE_SIZE;

    for (auto i = 0u; i < neededPages; ++i)
    {
        setPage(kernelPageTable, i, (i * PAGE_SIZE) | PGT_PRESENT | PGT_WRITEABLE);
    }

    setPageTable(
        kernelPageDir,
        pageTableIndex(KERNEL_PAGE_OFFSET),
        virt2phys(reinterpret_cast<uint32_t>(kernelPageTable)) | PGD_PRESENT | PGD_WRITEABLE);

    kernelAddressSpace_.pageDir = kernelPageDir;
    addressSpaces_.push_back(kernelAddressSpace_);

    clearPageTable(kernelPageDir, 0);

    loadPageDirectory(virt2phys(kernelPageDir));

    unmap(kernelPageDir, phys2virt(pageDir));
    pmm_->deallocateFrame(reinterpret_cast<uint32_t>(pageDir));

    unmap(kernelPageDir, phys2virt(page0));
    pmm_->deallocateFrame(reinterpret_cast<uint32_t>(page0));
}

} // namespace memory
