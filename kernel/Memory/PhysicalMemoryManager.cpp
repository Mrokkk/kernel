#include "PhysicalMemoryManager.hpp"

namespace memory
{

PhysicalMemoryManager::PhysicalMemoryManager(
    const uint32_t availableMemory,
    const uint32_t memoryToCover)
    : end_(memoryToCover)
    , size_(frameNumber(availableMemory))
    , log_(__func__)
{
    const auto count = frameNumber(memoryToCover);
    for (auto i = 0u; i < count; ++i)
    {
        data_[i] = true;
    }
}

PhysicalMemoryManager::Frame PhysicalMemoryManager::allocateFrame()
{
    uint32_t i;
    for (i = 0u; i < size_; ++i)
    {
        if (not data_[i])
        {
            break;
        }
    }
    data_[i] = true;
    auto f = Frame{i, frameAddress(i)};
    return f;
}

void PhysicalMemoryManager::deallocateFrame(const uint32_t address)
{
    data_[frameNumber(address)] = false;
}

uint32_t PhysicalMemoryManager::end() const
{
    return end_;
}

} // namespace memory
