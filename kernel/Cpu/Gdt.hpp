#pragma once

#include "Segment.hpp"

namespace cpu
{
struct Tss;

namespace gdt
{
namespace flags
{

enum Dpl
{
    Ring0 = 0 << 5,
    Ring1 = 1 << 5,
    Ring2 = 2 << 5,
    Ring3 = 3 << 5
};

enum Granularity
{
    u4kB = 1 << 8,
    u1B = 0 << 8
};

enum Size
{
    u16bit = 0 << 7,
    u32bit = 1 << 7
};

enum Type
{
    Code = 0x1a,
    Data = 0x12,
    Tss16 = 0x1,
    Tss32 = 0x9,
    IntGate16 = 0x6,
    IntGate32 = 0xe,
    TrapGate16 = 0x7,
    TrapGate32 = 0xf,
    TaskGate = 0x5
};

} // namespace flags

struct GdtEntry final
{
    constexpr GdtEntry(
        uint32_t flags,
        uint32_t base,
        uint32_t limit);

    uint32_t limit() const
    {
        return limitLow | (granularity & 0xf) << 16;
    }

    void limit(const uint32_t limit)
    {
        limitLow = limit & 0xffff;
        granularity = (limit >> 16) & 0xf;
    }

    uint32_t base() const
    {
        return baseLow | (baseMiddle << 16) | (baseHigh << 24);
    }

    void base(const uint32_t base)
    {
        baseLow = base & 0xffff;
        baseMiddle = (base >> 16) & 0xff;
        baseHigh = (base >> 24) & 0xff;
    }

    uint8_t type() const
    {
        return access & 0x1e;
    }

    uint16_t limitLow;
    uint16_t baseLow;
    uint8_t baseMiddle;
    uint8_t access;
    uint8_t granularity;
    uint8_t baseHigh;
} PACKED;

struct GdtRegister final
{
    uint16_t limit;
    GdtEntry* base;

    void load() const;
} PACKED;

void initialize();
void setTss(Tss& t);

} // namespace gdt
} // namespace cpu
