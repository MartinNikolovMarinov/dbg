#pragma once

#include <types.h>

namespace elf
{

/**
 * NOTE: The type informaiton is taken from the linux manual elf(5) entry.
 *
 * ELF - Executable and Linkable Format (ELF) files. The ELF header is always at offset zero of the file. The program
 * header table and the section header table's offset in the file are defined in the ELF header. The two tables describe
 * the rest of the particularities of the file.
*/

using namespace coretypes;

enum ElfClass : u8 {
    CLASSNONE = 0, // Invalid class
    ELF32 = 1,     // 32-bit objects
    ELF64 = 2      // 64-bit objects
};

enum ElfEncoding : u8 {
    DATANONE = 0,     // Unknown format
    LITTLEENDIAN = 1, // Little endian
    BIGENDIAN = 2     // Big endian
};

enum ElfOSAbi : u8 {
    UNIX = 0,         // UNIX System V ABI
    HPUX = 1,         // HP-UX
    NETBSD = 2,       // NetBSD
    LINUX = 3,        // Object uses GNU ELF extensions
    SOLARIS = 6,      // Sun Solaris
    AIX = 7,          // IBM AIX
    IRIX = 8,         // SGI Irix
    FREEBSD = 9,      // FreeBSD
    TRU64 = 10,       // Compaq TRU64 UNIX
    MODESTO = 11,     // Novell Modesto
    OPENBSD = 12,     // OpenBSD
    ARM_AEABI = 64,   // ARM EABI
    ARM = 97,         // ARM
    STANDALONE = 255, // Standalone (embedded) application
};

enum ElfType : u16 {
    ETNONE, // No file type
    ETREL,  // Relocatable file
    ETEXEC, // Executable file
    ETDYN,  // Shared object file
    ETCORE  // Core file
};

#pragma pack(push, 1)
struct ElfHeader32_Packed {
    union {
        // ident specifies how to interpret the file, independent of the processor or the file's remaining contents.
        u8 ident[16];
        struct {
            u8 magic0; // must be 0x7F
            u8 magic1; // must be 'E'
            u8 magic2; // must be 'L'
            u8 magic3; // must be 'F'
            ElfClass elfclass;
            ElfEncoding encoding;
            u8 fileversion; // must be 1
            ElfOSAbi os; // the operating system
            u8 abi; // the version of the abi
            /* 8 bytes of padding */
        };
    };
    ElfType type;
    u16 machine;
    u32 version;
    u32 entry;
    u32 phoff;
    u32 shoff;
    u32 flags;
    u16 ehsize;
    u16 phentsize;
    u16 phnum;
    u16 shentsize;
    u16 shnum;
    u16 shtrndx;
};
static_assert(sizeof(ElfHeader32_Packed) == 52);
#pragma pack(pop)

#pragma pack(push, 1)
struct ElfHeader64_Packed {
    union {
        // ident specifies how to interpret the file, independent of the processor or the file's remaining contents.
        u8 ident[16];
        struct {
            u8 magic0; // must be 0x7F
            u8 magic1; // must be 'E'
            u8 magic2; // must be 'L'
            u8 magic3; // must be 'F'
            ElfClass elfclass;
            ElfEncoding encoding;
            u8 fileversion; // must be 1
            ElfOSAbi os; // the operating system
            u8 abi; // the version of the abi
            /* 8 bytes of padding */
        };
    };
    ElfType type;
    u16 machine;
    u32 version;
    u64 entry;
    u64 phoff;
    u64 shoff;
    u32 flags;
    u16 ehsize;
    u16 phentsize;
    u16 phnum;
    u16 shentsize;
    u16 shnum;
    u16 shtrndx;
};
static_assert(sizeof(ElfHeader64_Packed) == 64);
#pragma pack(pop)

} // namespace elf
