#include "Gdt.hpp"

#include "kernel/Logger/Logger.hpp"

#include "Tss.hpp"

namespace cpu
{
namespace gdt
{

namespace
{

constexpr uint16_t gdtLowLimit(uint32_t limit)
{
    return limit & 0xffff;
}

constexpr uint32_t gdtHiLimit(uint32_t limit)
{
    return (limit >> 16) & 0xf;
}

constexpr uint16_t gdtLowBase(uint32_t base)
{
    return base & 0xffff;
}

constexpr uint8_t gdtMidBase(uint32_t base)
{
    return (base >> 16) & 0xff;
}

constexpr uint8_t gdtHiBase(uint32_t base)
{
    return (base >> 24) & 0xff;
}

constexpr uint8_t gdtLowFlags(uint32_t flags)
{
    return (flags & 0x7f) & 0xff;
}

constexpr uint8_t gdtHiFlags(uint32_t flags)
{
    return (flags >> 1) & 0xf0;
}

} // namespace

constexpr GdtEntry::GdtEntry(
    uint32_t flags,
    uint32_t base,
    uint32_t limit)
    : limitLow(gdtLowLimit(limit))
    , baseLow(gdtLowBase(base))
    , baseMiddle(gdtMidBase(base))
    , access(gdtLowFlags(flags) | (1 << 7))
    , granularity(gdtHiLimit(limit) | gdtHiFlags(flags))
    , baseHigh(gdtHiBase(base))
{
}

GdtEntry gdt_entries[] =
{
    // Null segment
    GdtEntry(0, 0, 0),

    // Kernel code segment
    GdtEntry(flags::Dpl::Ring0 | flags::Type::Code | flags::Granularity::u4kB | flags::Size::u32bit, 0x0, 0xffffffff),

    // Kernel data segment
    GdtEntry(flags::Dpl::Ring0 | flags::Type::Data | flags::Granularity::u4kB | flags::Size::u32bit, 0x0, 0xffffffff),

    // User code segment
    GdtEntry(flags::Dpl::Ring3 | flags::Type::Code | flags::Granularity::u4kB | flags::Size::u32bit, 0x0, 0xffffffff),

    // User data segment
    GdtEntry(flags::Dpl::Ring3 | flags::Type::Data | flags::Granularity::u4kB | flags::Size::u32bit, 0x0, 0xffffffff),

    // TSS
    GdtEntry(flags::Type::Tss32, 0, sizeof(Tss) - sizeof(Tss::ioBitmap))
};

GdtRegister gdt{sizeof(gdt_entries) - 1, gdt_entries};

void GdtRegister::load() const
{
    asm volatile(R"(
            lgdt (%%eax)
            ljmp $0x08, $1f
        1:
    )" :: "a" (this));
}

void initialize()
{
    Logger("Gdt") << info << __func__;

    gdt.load();
    asm volatile(R"(
        mov %0, %%ds
        mov %0, %%es
        mov %0, %%fs
        mov %0, %%gs
    )" :: "r" (cpu::Segment::KernelDs));
}

void setTss(Tss& t)
{
    gdt_entries[5].base(reinterpret_cast<uint32_t>(&t));
    gdt_entries[5].access &= 0xf9; // Clear busy bit
    t.load();
}

} // namespace gdt
} // namespace cpu
