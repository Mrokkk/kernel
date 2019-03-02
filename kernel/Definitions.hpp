#pragma once

#ifdef __ASSEMBLER__

#define ENTRY(name) \
    .global name; \
    name:

#define ALIGN(x) \
    .align x

#define SECTION(sec) \
    .section sec

#else

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>

int vsprintf(char* buf, const char* fmt, va_list args);
int sprintf(char* buf, const char* fmt, ...);

inline constexpr void* operator new(size_t, void* address)
{
    return address;
}

inline constexpr void* operator new[](size_t, void* address)
{
    return address;
}

#ifdef __cplusplus
#define asmlinkage extern "C"
#else
#define asmlinkage
#endif

#define ALIGN(x) __attribute__((aligned(x)))
#define PACKED __attribute__((packed))
#define SECTION(x) __attribute__ ((section(#x)))
#define FASTCALL(x) __attribute__((regparm(3))) x
#define NORETURN __attribute__((noreturn))

#define EXPAND(x) x
#define PASTE(a, b) a##b

#define __STRINGIFY(x) #x
#define STRINGIFY(x) __STRINGIFY(x)

#define __off_t_defined
using off_t = int;

#endif
