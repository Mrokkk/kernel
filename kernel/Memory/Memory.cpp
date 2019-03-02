#include "Memory.hpp"

#include <lazy_instance.hpp>

#include "kernel/Boot/Boot.hpp"
#include "kernel/Logger/Logger.hpp"

#include "Allocator.hpp"
#include "Sections.hpp"
#include "VirtualMemoryManager.hpp"
#include "PhysicalMemoryManager.hpp"

namespace memory
{

yacppl::lazy_instance<PhysicalMemoryManager> pmm;
yacppl::lazy_instance<VirtualMemoryManager> vmm;
yacppl::lazy_instance<Allocator<VirtualMemoryManager, memoryBlockSize>> heapAllocator;
struct MemoryMap memoryMap[16];

void initialize()
{
    pmm.initialize(
        align(boot::bootData.upperMemory, 1024) * 1024,
        reinterpret_cast<uint32_t>(virt2phys(sections::__heap_start)));

    vmm.initialize(pmm.get());

    heapAllocator.initialize(vmm.get());

    Logger logger("Memory");

    for (auto i = 0u; i < boot::bootData.memoryMapSize; ++i)
    {
        logger << info << "[" << (int)i << "] = " << memoryMap[i];
    }
}

LineWrapper& operator<<(LineWrapper& stream, MemoryMap::Type type)
{
    switch (type)
    {
        case MemoryMap::Type::Available:
            return stream << "Available";
        case MemoryMap::Type::Device:
            return stream << "Device";
        case MemoryMap::Type::Unavailable:
            return stream << "Unavailable";
        default:
            return stream << "Unknown";
    }
    return stream;
}

LineWrapper& operator<<(LineWrapper& stream, const MemoryMap& memoryMap)
{
    return stream << "MemoryMap{"
                  << "base=" << memoryMap.base << ", "
                  << "size=" << memoryMap.size << ", "
                  << "type=" << memoryMap.type << "}";

}

} // namespace memory
