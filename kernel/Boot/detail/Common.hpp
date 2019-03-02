#pragma once

namespace boot
{
struct Data;

namespace detail
{

void strcpy(const char* from, char* to);
uint32_t* physicalAddress(uint32_t* a);
char* physicalAddress(char* a);
Data* bootDataPhysical();

} // namespace detail
} // namespace boot
