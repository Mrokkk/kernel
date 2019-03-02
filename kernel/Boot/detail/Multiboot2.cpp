#include "kernel/Memory/Memory.hpp"

#include "../Boot.hpp"
#include "../Multiboot.hpp"
#include "Common.hpp"

namespace boot
{
namespace detail
{

SECTION(.boot)
enum memory::MemoryMap::Type convert_memory_type(const uint32_t type)
{
    switch (type)
    {
        case 1:
            return memory::MemoryMap::Type::Available;
        case 2:
            return memory::MemoryMap::Type::Unavailable;
        case 3:
            return memory::MemoryMap::Type::Device;
        case 4:
            return memory::MemoryMap::Type::Device;
        default:
            return memory::MemoryMap::Type::Unknown;
    }
}

asmlinkage SECTION(.boot)
void readBootloaderData(void* tag)
{
    auto module_index = 0u;
    auto bootData = bootDataPhysical();

    auto temp = reinterpret_cast<mb2_tag*>(static_cast<mb2_tags_header*>(tag) + 1);

    for (; temp->type != 0; temp = reinterpret_cast<mb2_tag*>(((char*)temp + ((temp->size + 7) & ~7))))
    {
        switch (temp->type)
        {
            case MULTIBOOT_TAG_TYPE_CMDLINE:
            {
                strcpy(reinterpret_cast<char*>(temp + 1), bootData->cmdline);
                break;
            }
            case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
            {
                strcpy(reinterpret_cast<char*>(temp + 1), bootData->bootloaderName);
                break;
            }
            case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO:
            {
                bootData->upperMemory = reinterpret_cast<uint32_t*>(temp)[3];
                bootData->lowerMemory = reinterpret_cast<uint32_t*>(temp)[2];
                break;
            }
            case MULTIBOOT_TAG_TYPE_MODULE:
            {
                auto module_tag = reinterpret_cast<mb2_module_tag*>(temp);
                auto modules_array = reinterpret_cast<Module*>(reinterpret_cast<uint32_t>(::boot::modules) - KERNEL_PAGE_OFFSET);
                modules_array[module_index].start = module_tag->mod_start;
                modules_array[module_index].end = module_tag->mod_end;
                strcpy(module_tag->name, modules_array[module_index].name);
                ++module_index;
                break;
            }
            case MULTIBOOT_TAG_TYPE_ELF_SECTIONS:
            {
                bootData->elfSymbols = true;
                break;
            }
            case MULTIBOOT_TAG_TYPE_MMAP:
            {
                const auto size = reinterpret_cast<uint32_t*>(temp)[1];
                const auto entry_size = reinterpret_cast<uint32_t*>(temp)[2];
                auto i = 0;
                const auto mmap = reinterpret_cast<memory::MemoryMap*>(
                    reinterpret_cast<uint32_t>(memory::memoryMap) - KERNEL_PAGE_OFFSET);
                auto entry = reinterpret_cast<mb2_mmap_entry*>(((uint32_t*)temp + 4));
                for ( ; entry < reinterpret_cast<mb2_mmap_entry*>((char*)temp + size); ++i)
                {
                    mmap[i].base = static_cast<uint32_t>(entry->base_addr);
                    mmap[i].size = static_cast<uint32_t>(entry->length);
                    mmap[i].type = convert_memory_type(entry->type);
                    entry = reinterpret_cast<mb2_mmap_entry*>((char*)entry + entry_size);
                }
                temp = reinterpret_cast<mb2_tag*>(entry);
                bootData->memoryMapSize = i;
                break;
            }
            case MULTIBOOT_TAG_TYPE_APM:
            {
                bootData->apmTableAddress = reinterpret_cast<uint32_t>(temp) + sizeof(uint32_t);
                break;
            }
            default:
                break;
        }
    }
}

} // namespace detail
} // namespace boot
