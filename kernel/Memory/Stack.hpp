#pragma once

namespace memory
{

struct Stack
{
    Stack(
        const uint32_t stackAddress,
        const uint32_t stackSize);

    Stack(const Stack&) = delete;
    Stack& operator=(const Stack&) = delete;

    ~Stack();

    uint32_t start() const
    {
        return stackAddress_;
    }

    uint32_t end() const
    {
        return stackAddress_ + stackSize_;
    }

    static Stack make(const size_t size);

private:
    const uint32_t stackAddress_;
    const uint32_t stackSize_;
};

} // namespace memory
