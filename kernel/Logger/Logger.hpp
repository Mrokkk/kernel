#pragma once

#include <string.hpp>
#include <spinlock.hpp>

struct Writer;

enum LogLevel
{
    debug, info, warning, error
};

struct Logger;

struct LineWrapper
{
    template <typename T>
    typename yacppl::enable_if<
        not yacppl::is_same<typename yacppl::remove_const<T>::type, char>::value, LineWrapper&
    >::type operator<<(const T* a)
    {
        index_ += sprintf(&buffer_[index_], "0x%08x", reinterpret_cast<uint32_t>(a));
        return *this;
    }

    LineWrapper& operator<<(const char* str);
    LineWrapper& operator<<(char str[]);
    LineWrapper& operator<<(int a);
    LineWrapper& operator<<(uint64_t a);
    LineWrapper& operator<<(uint32_t a);
    LineWrapper& operator<<(uint16_t a);
    LineWrapper& operator<<(uint8_t a);
    LineWrapper& operator<<(char c);
    LineWrapper& operator<<(bool b);

    ~LineWrapper();

    friend Logger;

private:
    explicit LineWrapper(
        uint32_t timestamp,
        LogLevel logLevel,
        const char* componentName);

    char buffer_[256];
    size_t index_;
};

struct Logger
{
    Logger(const char* componentName);

    void initialize(Writer* writer);

    LineWrapper operator<<(const LogLevel logLevel);

    friend LineWrapper;

private:
    static void print(const char* c);

    constexpr static const size_t bufferSize = 8192;

    static Logger instance_;
    static yacppl::spinlock spinlock_;
    static char data_[bufferSize];
    static size_t index_;
    const char* componentName_;
    static Writer* writer_;
};
