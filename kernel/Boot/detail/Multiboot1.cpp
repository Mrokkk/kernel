#include "kernel/Memory/Memory.hpp"

#include "../Boot.hpp"
#include "../Multiboot.hpp"
#include "Common.hpp"

namespace boot
{
namespace detail
{

SECTION(.boot)
memory::MemoryMap::Type convertMemoryType(const uint32_t type)
{
    switch (type)
    {
        case 1: return memory::MemoryMap::Type::Available;
        case 2: return memory::MemoryMap::Type::Unavailable;
        case 3: return memory::MemoryMap::Type::Device;
        case 4: return memory::MemoryMap::Type::Device;
        default: return memory::MemoryMap::Type::Unknown;
    }
}

SECTION(.boot)
void readMemoryMap(uint32_t, uint32_t mmap_address)
{
    memory_map* mm;
    int i;
    const auto mmap = reinterpret_cast<memory::MemoryMap*>(
        reinterpret_cast<uint32_t>(memory::memoryMap) - KERNEL_PAGE_OFFSET);

    for (mm = (struct memory_map*)mmap_address, i = 0;
         mm->base_addr_low + (mm->length_low - 1) != 0xffffffff;
         ++i)
    {
        mmap[i].type = convertMemoryType(mm->type);
        mmap[i].base = mm->base_addr_low;
        mmap[i].size = mm->length_low;
        mm = (memory_map*)((unsigned int)mm + mm->size + 4);
    }

    bootDataPhysical()->memoryMapSize = i;
}

SECTION(.boot)
bool doModulesExist(const multiboot_info* multibootData)
{
    return (multibootData->flags & MULTIBOOT_FLAGS_MODS_BIT) and multibootData->mods_count;
}

asmlinkage SECTION(.boot)
void readBootloaderData(void* tag)
{
    auto bootData = bootDataPhysical();

    const auto multibootData = static_cast<multiboot_info*>(tag);

    strcpy(multibootData->cmdline, bootData->cmdline);
    strcpy(multibootData->bootloader_name, bootData->bootloaderName);

    bootData->elfSymbols = static_cast<bool>(multibootData->flags & MULTIBOOT_FLAGS_SYMS_BIT);

    bootData->lowerMemory = multibootData->mem_lower;
    bootData->upperMemory = multibootData->mem_upper;

    readMemoryMap(multibootData->mmap_length, multibootData->mmap_addr);
    bootData->apmTableAddress = multibootData->apm_table;

    if (doModulesExist(multibootData))
    {
        auto modulesArray = reinterpret_cast<Module*>(reinterpret_cast<uint32_t>(::boot::modules) - KERNEL_PAGE_OFFSET);
        auto module = reinterpret_cast<::module*>(multibootData->mods_addr);
        for (auto moduleIndex = 0u; moduleIndex < multibootData->mods_count; ++moduleIndex, ++module)
        {
            modulesArray[moduleIndex].start = module->mod_start;
            modulesArray[moduleIndex].end = module->mod_end;
            strcpy(module->string, modulesArray[moduleIndex].name);
        }
    }
}

} // namespace detail
} // namespace boot
