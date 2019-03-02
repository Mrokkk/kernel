#include "kernel/Memory/Memory.hpp"
#include "../Boot.hpp"
#include "Common.hpp"

namespace boot
{
namespace detail
{

SECTION(.boot)
void strcpy(const char* from, char* to)
{
    while (*from)
    {
        *to++ = *from++;
    }
    *to = 0;
}

SECTION(.boot)
uint32_t *physicalAddress(uint32_t* a)
{
    return reinterpret_cast<uint32_t*>(reinterpret_cast<uint32_t>(a) - KERNEL_PAGE_OFFSET);
}

SECTION(.boot)
char *physicalAddress(char* a)
{
    return reinterpret_cast<char*>(reinterpret_cast<uint32_t>(a) - KERNEL_PAGE_OFFSET);
}

SECTION(.boot)
Data* bootDataPhysical()
{
    return reinterpret_cast<Data*>(reinterpret_cast<uint32_t>(&bootData) - KERNEL_PAGE_OFFSET);
}

} // namespace detail
} // namespace boot
