#include "Pic.hpp"

#include "kernel/Logger/Logger.hpp"

#include "Io.hpp"
#include "IrqLock.hpp"

namespace cpu
{
namespace pic
{

constexpr const auto PIC1 = 0x20u;
constexpr const auto PIC2 = 0xA0u;

constexpr const auto ICW2_PIC1 = 0x20u;
constexpr const auto ICW2_PIC2 = 0x28u;

using namespace io;

namespace
{

uint16_t mask = 0xffff;

void picDisable()
{
    auto _ = IrqLock::make();
    outb(0xff, PIC1 + 1);
    outb(0xff, PIC2 + 1);
}

inline void sendIcw1()
{
    outb(0x11, PIC1);
    outb(0x11, PIC2);
}

inline void sendIcw2()
{
    outb(ICW2_PIC1, PIC1 + 1);
    outb(ICW2_PIC2, PIC2 + 1);
}

inline void sendIcw3()
{
    outb(4, PIC1 + 1);
    outb(2, PIC2 + 1);
}

inline void sendIcw4()
{
    outb(1, PIC1 + 1);
    outb(1, PIC2 + 1);
}

} // namespace

void initialize()
{
    Logger("Pic") << info << __func__;
    sendIcw1();
    sendIcw2();
    sendIcw3();
    sendIcw4();
    picDisable();
}

void enable(const uint32_t irq)
{
    auto _ = IrqLock::make();

    mask &= ~(1 << irq);
    if (irq < 8)
    {
        outb(mask & 0xff, PIC1 + 1);
    }
    else
    {
        outb(mask >> 8, PIC2 + 1);
    }
}

} // namespace pic
} // namespace cpu
