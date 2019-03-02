#pragma once

#ifndef __ASSEMBLER__

struct mb2_tags_header
{
    uint32_t size;
    uint32_t reserved;
} PACKED;

struct mb2_tag
{
    uint32_t type;
    uint32_t size;
} PACKED;

struct mb2_module_tag
{
    uint32_t type;
    uint32_t size;
    uint32_t mod_start;
    uint32_t mod_end;
    char* name;
} PACKED;

struct mb2_mmap_entry
{
    uint64_t base_addr;
    uint64_t length;
    uint32_t type;
    uint32_t reserved;
} PACKED;

#endif

#define MULTIBOOT_TAG_TYPE_END               0
#define MULTIBOOT_TAG_TYPE_CMDLINE           1
#define MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME  2
#define MULTIBOOT_TAG_TYPE_MODULE            3
#define MULTIBOOT_TAG_TYPE_BASIC_MEMINFO     4
#define MULTIBOOT_TAG_TYPE_BOOTDEV           5
#define MULTIBOOT_TAG_TYPE_MMAP              6
#define MULTIBOOT_TAG_TYPE_VBE               7
#define MULTIBOOT_TAG_TYPE_FRAMEBUFFER       8
#define MULTIBOOT_TAG_TYPE_ELF_SECTIONS      9
#define MULTIBOOT_TAG_TYPE_APM               10

#define MULTIBOOT_HEADER_MAGIC 0xe85250d6
#define MULTIBOOT_HEADER_ARCH  0

#define MULTIBOOT_HEADER() \
    header_start: \
    .long MULTIBOOT_HEADER_MAGIC; \
    .long MULTIBOOT_HEADER_ARCH; \
    .long header_end - header_start; \
    .long -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_ARCH + header_end - header_start); \
    .word 1; \
    .word 0; \
    .long 4+8; \
    .long 1; \
    .balign 8; \
    .word 0; \
    .word 0; \
    .long 8; \
    header_end:

#define MULTIBOOT_BOOTLOADER_MAGIC 0x36d76289

