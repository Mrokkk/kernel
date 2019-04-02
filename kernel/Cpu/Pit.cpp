#include "Pit.hpp"

#include "kernel/Logger/Logger.hpp"

#include "Io.hpp"

namespace cpu
{
using namespace io;

namespace pit
{

constexpr const auto PIT_PORT_CHANNEL0 = 0x40;
constexpr const auto PIT_PORT_CHANNEL1 = 0x41;
constexpr const auto PIT_PORT_CHANNEL2 = 0x42;
constexpr const auto PIT_PORT_COMMAND = 0x43;

constexpr const auto PIT_16BIN = 0;
constexpr const auto PIT_BCD = 1;

enum Mode : uint8_t
{
    Mode0 = (0 << 1),
    Mode1 = (1 << 1),
    Mode2 = (2 << 1),
    Mode2b = (6 << 1),
    Mode3 = (3 << 1),
    Mode3b = (7 << 1),
    Mode4 = (4 << 1),
    Mode5 = (5 << 1)
};

constexpr const auto PIT_LATCH_CVAL = (0 << 4);
constexpr const auto PIT_ACCESS_LO = (1 << 4);
constexpr const auto PIT_ACCESS_HI = (2 << 4);
constexpr const auto PIT_ACCES_LOHI = (3 << 4);

enum Channel : uint8_t
{
    Channel0 = (0 << 6),
    Channel1 = (1 << 6),
    Channel2 = (2 << 6)
};

constexpr const auto PIT_READ_BACK = (3 << 6);

constexpr const auto CLOCK_TICK_RATE = 1193182;
constexpr const auto LATCH = ((CLOCK_TICK_RATE) / HZ);

void initialize()
{
    Logger("Pit") << info << __func__ << ": configuring for " << HZ << " HZ; "
                                      << "mode: " << (Mode2 >> 1) << "; "
                                      << "clock tick rate: " << CLOCK_TICK_RATE;

    outb(Channel0 | Mode2 | PIT_ACCES_LOHI | PIT_16BIN, PIT_PORT_COMMAND);
    outb(LATCH & 0xff, PIT_PORT_CHANNEL0);
    outb(LATCH >> 8, PIT_PORT_CHANNEL0);
}

} // namespace pit
} // namespace cpu
