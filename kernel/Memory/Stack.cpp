#include "Stack.hpp"

#include "kernel/Memory/VirtualMemoryManager.hpp"

namespace memory
{

Stack::Stack(
    const uint32_t stackAddress,
    const uint32_t stackSize)
    : stackAddress_(stackAddress)
    , stackSize_(stackSize)
{
}

Stack::~Stack()
{
    //delete [] reinterpret_cast<uint32_t*>(stackAddress_);
}

Stack Stack::make(const size_t size)
{
    return Stack(
        reinterpret_cast<uint32_t>(vmm->allocate()),
        size);
}

} // namespace memory
