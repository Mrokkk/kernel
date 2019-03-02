#pragma once

#include "Writer.hpp"

struct SerialOutput : public Writer
{
    enum Port {
        Com1 = 0x3f8,
        Com2 = 0x2f8,
        Com3 = 0x3e8,
        Com4 = 0x2e8
    };

    SerialOutput(const Port port);
    int write(const char* buffer, size_t n) override;
    static void send(char c, const Port port);

private:
    static int isTransmitEmpty(const Port port);

    Port port_;
};
