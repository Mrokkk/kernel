#pragma once

namespace memory
{
namespace sections
{

asmlinkage char __text_start[];
asmlinkage char __text_end[];
asmlinkage char __data_start[];
asmlinkage char __data_end[];
asmlinkage char __bss_start[];
asmlinkage char __bss_end[];
asmlinkage char __heap_start[];

} // namespace sections
} // namespace memory
