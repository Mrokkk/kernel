#include "Logger.hpp"
#include "kernel/Writer.hpp"

extern volatile size_t jiffies;

Logger Logger::instance_("");
yacppl::spinlock Logger::spinlock_;
char Logger::data_[Logger::bufferSize];
size_t Logger::index_;
Writer* Logger::writer_;

const char* logLevelToString(const LogLevel logLevel)
{
    switch (logLevel)
    {
        case LogLevel::debug:
            return "DBG";
        case LogLevel::info:
            return "INF";
        case LogLevel::warning:
            return "WRN";
        case LogLevel::error:
            return "ERR";
        default:
            return "DBG";
    }
}

LineWrapper::LineWrapper(
    uint32_t timestamp,
    LogLevel logLevel,
    const char* componentName)
    : index_(0u)
{
    index_ += sprintf(buffer_, "[0x%08x][%s] [%s] ", timestamp, logLevelToString(logLevel), componentName);
}

LineWrapper::~LineWrapper()
{
    buffer_[index_++] = '\n';
    buffer_[index_] = 0;
    Logger::print(buffer_);
}

Logger::Logger(const char* componentName)
    : componentName_(componentName)
{
    if (index_ == 0u)
    {
        *data_ = 0;
    }
}

void Logger::initialize(Writer* writer)
{
    writer_ = writer;
    writer_->write(data_, yacppl::length(data_));
}

void Logger::print(const char* string)
{
    const auto len = yacppl::length(string);
    if (writer_)
    {
        writer_->write(string, len);
        return;
    }

    yacppl::copy(string, data_ + index_);
    index_ += len;
}

LineWrapper Logger::operator<<(const LogLevel logLevel)
{
    return LineWrapper(jiffies, logLevel, componentName_);
}

LineWrapper& LineWrapper::operator<<(const char* str)
{
    yacppl::copy(str, &buffer_[index_]);
    index_ += yacppl::length(str);
    return *this;
}

LineWrapper& LineWrapper::operator<<(char str[])
{
    yacppl::copy(str, &buffer_[index_]);
    index_ += yacppl::length(str);
    return *this;
}

LineWrapper& LineWrapper::operator<<(int a)
{
    index_ += sprintf(&buffer_[index_], "%d", a);
    return *this;
}

LineWrapper& LineWrapper::operator<<(uint32_t a)
{
    index_ += sprintf(&buffer_[index_], "0x%08x", a);
    return *this;
}

LineWrapper& LineWrapper::operator<<(uint64_t a)
{
    index_ += sprintf(&buffer_[index_], "0x%08x", a);
    return *this;
}

LineWrapper& LineWrapper::operator<<(uint16_t a)
{
    index_ += sprintf(&buffer_[index_], "0x%04x", a);
    return *this;
}

LineWrapper& LineWrapper::operator<<(uint8_t a)
{
    index_ += sprintf(&buffer_[index_], "0x%02x", a);
    return *this;
}

LineWrapper& LineWrapper::operator<<(char c)
{
    buffer_[index_++] = c;
    buffer_[index_] = 0;
    return *this;
}

LineWrapper& LineWrapper::operator<<(bool b)
{
    index_ += sprintf(&buffer_[index_], b ? "true" : "false");
    return *this;
}
