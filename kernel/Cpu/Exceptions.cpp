#include "kernel/Kernel.hpp"
#include "kernel/Logger/Logger.hpp"
#include "kernel/Process/Process.hpp"

#include "Registers.hpp"
#include "StackFrame.hpp"
#include "Instructions.hpp"

#define EXCEPTION_ERRNO(x)      __STRING_##x,
#define EXCEPTION_NO_ERRNO(x)   __STRING_##x,

namespace cpu
{
namespace exceptions
{

namespace
{
constexpr const char* exceptionNames[] =
{
    #include "Exceptions.hpp"
};
} // namespace

struct Frame
{
    struct Frame* next;
    void* returnAddress;
} PACKED;

int readStackTrace(void** stackTrace)
{
    auto frame = reinterpret_cast<Frame*>(registers::Ebp::get());
    int i = 0;
    while (frame)
    {
        stackTrace[i++] = frame->returnAddress;
        frame = frame->next;
    }
    return i;
}

asmlinkage void exceptionHandler(
    const uint32_t nr,
    const uint32_t errorCode,
    StackFrame frame)
{
    cpu::cli();

    Logger logger(__func__);

    logger << error << exceptionNames[nr] << " #" << static_cast<int>(errorCode);
    logger << error << "EAX=" << frame.eax << "; EBX=" << frame.ebx << "; ECX=" << frame.ecx << "; EDX=" << frame.edx;
    logger << error << "ESP=" << frame.ss << ':' << frame.esp << "; EIP=" << frame.cs << ':' << frame.eip;
    logger << error << "DS=" << frame.ds << "; ES=" << frame.es << "; FS=" << frame.fs << "; GS=" << frame.gs;
    logger << error << "EFLAGS=" << frame.eflags << "; IOPL=" << ((frame.eflags >> 12) & 0x3);
    logger << error << "CR2 = " << registers::Cr2::get();

    logger << error << "Stacktrace:";

    void* addresses[1024]{};
    auto size = readStackTrace(addresses);

    for (auto i = 0; i < size; ++i)
    {
        logger << error << i << "\t" << addresses[i];
    }

    auto& scheduler = Kernel::getScheduler();
    if (scheduler.getCurrentProcess().type != process::Process::Type::Kernel)
    {
        scheduler.exit(-1);
        return;
    }

    while (1)
    {
        cpu::halt();
    }
}

} // namespace exceptions
} // namespace cpu
