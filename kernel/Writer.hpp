#pragma once

struct Writer
{
    virtual int write(const char* buffer, size_t n) = 0;
};
