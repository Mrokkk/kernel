#pragma once

namespace boot
{

struct Module
{
    uint32_t start;
    uint32_t end;
    char name[128];
};

struct MemoryRegion
{
    uint32_t base;
    uint32_t size;
    uint32_t type;
};

struct Data
{
    char bootloaderName[32];
    char cmdline[256];
    uint32_t lowerMemory;
    uint32_t upperMemory;
    uint32_t modulesStart;
    uint32_t modulesEnd;
    size_t memoryMapSize;
    MemoryRegion memoryMap[10];
    uint32_t apmTableAddress;
    bool elfSymbols;
};

extern Module modules[];

extern Data bootData;

void printBootInfo();

} // namespace boot
