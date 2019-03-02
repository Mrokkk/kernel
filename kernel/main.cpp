#include "Kernel.hpp"

asmlinkage NORETURN void main()
{
    Kernel kernel;
    kernel.run();
    while (1);
}
