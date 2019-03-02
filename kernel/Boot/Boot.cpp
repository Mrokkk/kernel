#include "Boot.hpp"
#include "detail/Common.hpp"

#include "kernel/Logger/Logger.hpp"
#include "kernel/Memory/Memory.hpp"

namespace boot
{

Data bootData;
Module modules[4];

void printBootInfo()
{
    Logger log("Boot");

    log << info << "Bootloader name: " << bootData.bootloaderName;
    log << info << "Boot command-line: " << bootData.cmdline;
    log << info << "Lower mem: " << (int)(bootData.lowerMemory) << "kiB";
    log << info << "Upper mem: " << (int)(bootData.upperMemory / 1024) << "MiB";
    log << info << "Memory map size: " << (int)bootData.memoryMapSize;
    log << info << "APM table addr: " << bootData.apmTableAddress;
    log << info << "ELF symbols available: " << bootData.elfSymbols;

    for (auto i = 0u; modules[i].end != 0; ++i)
    {
        log << info << "Module: " << boot::modules[i].name << " @ " << boot::modules[i].start << " - " << boot::modules[i].end;
    }
}

} // namespace boot
