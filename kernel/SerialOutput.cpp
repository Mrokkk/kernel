#include "SerialOutput.hpp"

#include <string.hpp>

#include "Cpu/Io.hpp"
#include "Cpu/Reboot.hpp"
#include "Interrupt/Manager.hpp"
#include "kernel/Kernel.hpp"
#include "kernel/Logger/Logger.hpp"

using cpu::io::inb;
using cpu::io::outb;

namespace
{
char buffer[2048];
size_t index = 0u;
} // namespace

void isr(uint32_t nr, cpu::StackFrame*)
{
    auto port = nr == 4
        ? SerialOutput::Port::Com1
        : SerialOutput::Port::Com2;

    auto c = inb(port);

    if (c == 0x0d)
    {
        buffer[index++] = 0;
        Logger(__func__) << info << buffer;
        index = 0u;
        if (not yacppl::compare(buffer, "exit"))
        {
            cpu::reboot();
        }
    }
    else
    {
        buffer[index++] = c;
    }
}

SerialOutput::SerialOutput(const Port port)
    : port_(port)
{
    outb(0x00, port + 1);   /* Disable all interrupts*/
    outb(0x80, port + 3);   /* Enable DLAB (set baud rate divisor) */
    outb(0x01, port + 0);   /* Set divisor to 3 (lo byte) 38400 baud */
    outb(0x00, port + 1);   /*                  (hi byte) */
    outb(0x03, port + 3);   /* 8 bits, no parity, one stop bit */
    outb(0xc7, port + 2);   /* Enable FIFO, clear them, with 14-byte threshold */
    outb(0x0b, port + 4);   /* IRQs enabled, RTS/DSR set */
    outb(0x01, port + 1);   /* Enable interrupt for receiving */
    Kernel::getInterruptManager().registerHandler(4, &isr, "com1");
}

int SerialOutput::write(const char* buffer, size_t n)
{
    for (auto i = 0u; i < n; ++i)
    {
        send(buffer[i], port_);
    }
    return n;
}

int SerialOutput::isTransmitEmpty(const Port port)
{
    return inb(port + 5) & 0x20;
}

void SerialOutput::send(char a, const Port port)
{
    if (a == '\n')
    {
        send('\r', port);
    }

    while (isTransmitEmpty(port) == 0);
    outb(a, port);
}
