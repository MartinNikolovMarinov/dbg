#pragma once

#include <types.h>

/**
 * NOTE: The type informaiton is taken from the linux manual elf(5) entry.
 *
 * ELF - Executable and Linkable Format (ELF) files. The ELF header is always at offset zero of the file. The program
 * header table and the section header table's offset in the file are defined in the ELF header. The two tables describe
 * the rest of the particularities of the file.
*/

namespace elf
{

using namespace coretypes;

enum struct ElfClass : u8 {
    CLASSNONE, // Invalid class
    ELF32,     // 32-bit objects
    ELF64,     // 64-bit objects

    COUNT
};

enum struct ElfEncoding : u8 {
    DATANONE,     // Unknown format
    LITTLEENDIAN, // Little endian
    BIGENDIAN,    // Big endian

    COUNT
};

enum struct ElfOSAbi : u8 {
    UNIX,         // UNIX System V ABI
    HPUX,         // HP-UX
    NETBSD,       // NetBSD
    LINUX,        // Object uses GNU ELF extensions
    SOLARIS,      // Sun Solaris
    AIX,          // IBM AIX
    IRIX,         // SGI Irix
    FREEBSD,      // FreeBSD
    TRU64,       // Compaq TRU64 UNIX
    MODESTO,     // Novell Modesto
    OPENBSD,     // OpenBSD

    COUNT,

    // Reserved values:
    ARM_AEABI = 64,   // ARM EABI
    ARM = 97,         // ARM
    STANDALONE = 255, // Standalone (embedded) application
};

enum struct ElfType : u16 {
    ETNONE, // No file type
    ETREL,  // Relocatable file
    ETEXEC, // Executable file
    ETDYN,  // Shared object file
    ETCORE,  // Core file

    COUNT
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
            ElfClass elfclass; // identifies 64 or 32 bit architecture
            ElfEncoding encoding; // little or big endian
            u8 fileversion; // must be 1
            ElfOSAbi os; // the operating system
            u8 abi; // the version of the abi
            /* 8 bytes of padding */
        };
    };
    // Type of object file.
    ElfType type;
    // The specific machine architecture.
    u16 machine;
    // Same as fileversion.
    u32 version;
    // This member gives the virtual address to which the system first transfers control, thus starting the process.
    // If the file has no associated entry point, this member holds zero.
    u32 entry;
    // This member holds the program header table's file offset in bytes.  If the file has no program header table, this
    // member holds zero.
    u32 phoff;
    // This member holds the section header table's file offset in bytes.  If the file has no section header table, this
    // member holds zero.
    u32 shoff;
    // This member holds processor-specific flags associated with the file. Flag names take the form EF_"machine_flag".
    // Currently, no flags have been defined.
    u32 flags;
    // This member holds the ELF header's size in bytes.
    u16 ehsize;
    // This member holds the size in bytes of one entry in the file's program header table. All entries are the same size.
    u16 phentsize;
    // This member holds the number of entries in the program header table. Thus the product of phentsize and phnum
    // gives the table's size in bytes. If a file has no program header, phnum holds the value zero. If the number of
    // entries in the program header table is larger than or equal to PN_XNUM (0xffff), this member holds PN_XNUM
    // (0xffff) and the real number of entries in the program header table is held in the sh_info member of the initial
    // entry in section header table. Otherwise, the sh_info member of the initial entry contains the value zero.
    u16 phnum;
    // This member holds a sections header's size in bytes. A section header is one entry in the section header table.
    // All entries are the same size.
    u16 shentsize;
    // This member holds the number of entries in the section header table.  Thus the product of shentsize and shnum
    // gives the section header table's size in bytes.  If a file has no section header table, shnum holds the value
    // of zero. If the number of entries in the section header table is larger than or equal to SHN_LORESERVE (0xff00),
    // shnum holds the value zero and the real number of entries in the section header table is held in the sh_size
    // member of the initial entry in section header table.  Otherwise, the sh_size member of the initial entry in the
    // section header table holds the value zero.
    u16 shnum;
    // This member holds the section header table index of the entry associated with the section name string table. If
    // the file has no section name string table, this member holds the value SHN_UNDEF. If the index of section name
    // string table section is larger than or equal to SHN_LORESERVE (0xff00), this member holds SHN_XINDEX (0xffff) and
    // the real index of the section name string table section is held in the sh_link member of the initial entry in
    // section header table. Otherwise, the sh_link member of the initial entry in section header table contains the
    // value zero.
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
    // Type of object file.
    ElfType type;
    // The specific machine architecture.
    u16 machine;
    // Same as fileversion.
    u32 version;
    // This member gives the virtual address to which the system first transfers control, thus starting the process.
    // If the file has no associated entry point, this member holds zero.
    u64 entry;
    // This member holds the program header table's file offset in bytes. If the file has no program header table, this
    // member holds zero.
    u64 phoff;
    // This member holds the section header table's file offset in bytes. If the file has no section header table, this
    // member holds zero.
    u64 shoff;
    // This member holds processor-specific flags associated with the file. Flag names take the form EF_"machine_flag".
    // Currently, no flags have been defined.
    u32 flags;
    // This member holds the ELF header's size in bytes.
    u16 ehsize;
    // This member holds the size in bytes of one entry in the file's program header table. All entries are the same size.
    u16 phentsize;
    // This member holds the number of entries in the program header table. Thus the product of phentsize and phnum
    // gives the table's size in bytes. If a file has no program header, phnum holds the value zero. If the number of
    // entries in the program header table is larger than or equal to PN_XNUM (0xffff), this member holds PN_XNUM
    // (0xffff) and the real number of entries in the program header table is held in the sh_info member of the initial
    // entry in section header table. Otherwise, the sh_info member of the initial entry contains the value zero.
    u16 phnum;
    // This member holds a sections header's size in bytes. A section header is one entry in the section header table.
    // All entries are the same size.
    u16 shentsize;
    // This member holds the number of entries in the section header table.  Thus the product of shentsize and shnum
    // gives the section header table's size in bytes.  If a file has no section header table, shnum holds the value
    // of zero. If the number of entries in the section header table is larger than or equal to SHN_LORESERVE (0xff00),
    // shnum holds the value zero and the real number of entries in the section header table is held in the sh_size
    // member of the initial entry in section header table.  Otherwise, the sh_size member of the initial entry in the
    // section header table holds the value zero.
    u16 shnum;
    // This member holds the section header table index of the entry associated with the section name string table. If
    // the file has no section name string table, this member holds the value SHN_UNDEF. If the index of section name
    // string table section is larger than or equal to SHN_LORESERVE (0xff00), this member holds SHN_XINDEX (0xffff) and
    // the real index of the section name string table section is held in the sh_link member of the initial entry in
    // section header table. Otherwise, the sh_link member of the initial entry in section header table contains the
    // value zero.
    u16 shtrndx;
};
static_assert(sizeof(ElfHeader64_Packed) == 64);
#pragma pack(pop)

/*
Elf Program Header Type:

PT_NULL
    The array element is unused and the other members' values are undefined.  This lets the program header have ignored
    entries.

PT_LOAD
    The array element specifies a loadable segment, described by p_filesz and p_memsz.  The bytes from the file are
    mapped to the beginning of the memory segment.  If the segment's memory size p_memsz is larger than the file size
    p_filesz, the "extra" bytes are defined to hold the value 0 and to follow the segment's initialized area. The file
    size may not be larger than the memory size.  Loadable segment entries in the program header table appear in
    ascending order, sorted on the p_vaddr member.

PT_DYNAMIC
    The array element specifies dynamic linking information.

PT_INTERP
    The array element specifies the location and size of a null-terminated pathname to invoke as an interpreter.  This
    segment type is meaningful only for executable files (though it may occur for shared objects).  However it may not
    occur more than once in a file.  If it is present, it must precede any loadable segment entry.

PT_NOTE
    The array element specifies the location of notes (ElfN_Nhdr).

PT_SHLIB
    This segment type is reserved but has unspecified semantics.  Programs that contain an array element of this type do
    not conform to the ABI.

PT_PHDR
    The array element, if present, specifies the location and size of the program header table itself, both in the file
    and in the memory image of the program.  This segment type may not occur more than once in a file.  Moreover, it may
    occur only if the program header table is part of the memory image of the program.  If it is present, it must
    precede any loadable segment entry.

PT_LOPROC, PT_HIPROC
    Values in the inclusive range [PT_LOPROC, PT_HIPROC] are reserved for processor-specific semantics.

PT_GNU_STACK
    GNU extension which is used by the Linux kernel to control the state of the stack via the flags set in the p_flags
    member.
*/
enum struct ElfProgHeaderType : u32 {
    NUL,
    LOAD,
    DYNAMIC,
    INTERP,
    NOTE,
    SHLIB,
    PHDR,
    TLS,

    COUNT,

    // Reserved values:
    LOOS   = 0x60000000, // Start of OS specific
    HIOS   = 0x6fffffff, // End OS-specific type.
    LOPROC = 0x70000000, // Start of processor-specific
    HIPROC = 0x7fffffff, // End of processor-specific
    LOUSER = 0x80000000, // Start of application-specific
    HIUSER = 0x8fffffff, // End of application-specific
};

enum struct ElfProgHeaderFlags : u32 {
    E = 1 << 0,  // An executable segment.
    W = 1 << 1,  // A writable segment.
    R = 1 << 2,  // A readable segment.
    WE = W | E,
    RE = R | E,
    RW = R | W,
    RWE = R | W | E,
};

#pragma pack(push, 1)
struct ElfProgHeader32_Packed {
    // This member of the structure indicates what kind of segment this array element describes or how to interpret the
    // array element's information.
    ElfProgHeaderType type;
    // This member holds the offset from the beginning of the file at which the first byte of the segment resides.
    u32 offset;
    // This member holds the virtual address at which the first byte of the segment resides in memory.
    u32 vaddr;
    // On systems for which physical addressing is relevant, this member is reserved for the segment's physical address.
    // Under BSD this member is not used and must be zero.
    u32 paddr;
    // This member holds the number of bytes in the file image of the segment.  It may be zero.
    u32 filesz;
    // This member holds the number of bytes in the memory image of the segment. It may be zero.
    u32 memsz;
    // This member holds a bit mask of flags relevant to the segment:
    ElfProgHeaderFlags flags;
    // This member holds the value to which the segments are aligned in memory and in the file. Loadable process
    // segments must have congruent values for p_vaddr and p_offset, modulo the page size. Values of zero and one mean
    // no alignment is required. Otherwise, p_align should be a positive, integral power of two, and p_vaddr should
    // equal p_offset, modulo p_align.
    u32 align;
};
static_assert(sizeof(ElfProgHeader32_Packed) == 32);
#pragma pack(pop)

#pragma pack(push, 1)
struct ElfProgHeader64_Packed {
    // This member of the structure indicates what kind of segment this array element describes or how to interpret the
    // array element's information.
    ElfProgHeaderType type;
    // This member holds a bit mask of flags relevant to the segment:
    // PF_X - An executable segment.
    // PF_W - A writable segment.
    // PF_R - A readable segment.
    // A text segment commonly has the flags PF_X and PF_R. A data segment commonly has PF_W and PF_R.
    ElfProgHeaderFlags flags;
    // This member holds the offset from the beginning of the file at which the first byte of the segment resides.
    u64 offset;
    // This member holds the virtual address at which the first byte of the segment resides in memory.
    u64 vaddr;
    // On systems for which physical addressing is relevant, this member is reserved for the segment's physical address.
    // Under BSD this member is not used and must be zero.
    u64 paddr;
    // This member holds the number of bytes in the file image of the segment. It may be zero.
    u64 filesz;
    // This member holds the number of bytes in the memory image of the segment. It may be zero.
    u64 memsz;
    // This member holds the value to which the segments are aligned in memory and in the file. Loadable process
    // segments must have congruent values for p_vaddr and p_offset, modulo the page size. Values of zero and one mean
    // no alignment is required. Otherwise, p_align should be a positive, integral power of two, and p_vaddr should
    // equal p_offset, modulo p_align.
    u64 align;
};
static_assert(sizeof(ElfProgHeader64_Packed) == 56);
#pragma pack(pop)

// SECTION HEAEADER (Shdr)

enum ElfSectionType : u32 {
    // This value marks the section header as inactive. It does not have an associated section. Other members of the
    // section header have undefined values.
    NUL,
    // This section holds information defined by the program, whose format and meaning are determined solely by the
    // program.
    PROGBITS,
    // This section holds a symbol table. Typically, SHT_SYMTAB provides symbols for link editing, though it may also be
    // used for dynamic linking. As a complete symbol table, it may contain many symbols unnecessary for dynamic
    // linking. An object file can also contain a SHT_DYNSYM section.
    SYMTAB,
    // This section holds a string table. An object file may have multiple string table sections.
    STRTAB,
    // This section holds relocation entries with explicit addends, such as type Elf32_Rela for the 32-bit class of
    // object files.  An object may have multiple relocation sections.
    RELA,
    // This section holds a symbol hash table.  An object participating in dynamic linking must contain a symbol hash
    // table. An object file may have only one hash table.
    HASH,
    // This section holds information for dynamic linking. An object file may have only one dynamic section.
    DYNAMIC,
    // This section holds notes (ElfN_Nhdr).
    NOTE,
    // A section of this type occupies no space in the file but otherwise resembles SHT_PROGBITS. Although this section
    // contains no bytes, the sh_offset member contains the conceptual file offset.
    NOBITS,
    // This section holds relocation offsets without explicit addends, such as type Elf32_Rel for the 32-bit class of
    // object files.  An object file may have multiple relocation sections.
    REL,
    // This section is reserved but has unspecified semantics.
    SHLIB,
    // This section holds a minimal set of dynamic linking symbols.  An object file can also contain a SHT_SYMTAB
    // section.
    DYNSYM,

    COUNT,

    // Reserved values:

    LOOS   = 0x60000000, // Start OS-specific.
    LOSUNW = 0x6ffffffa, // Start Sun-specific.
    HISUNW = 0x6fffffff, // End Sun-specific.
    HIOS   = 0x6fffffff, // End OS-specific type
    LOPROC = 0x70000000, // Start of processor-specific
    HIPROC = 0x7fffffff, // End of processor-specific
    LOUSER = 0x80000000, // Start of user-specific
    HIUSER = 0x8fffffff, // End of user-specific
};

#pragma pack(push, 1)
struct ElfSectionHeader32_Packed {
    // This member specifies the name of the section. Its value is an index into the section header string table
    // section, giving the location of a null-terminated string.
    u32 name;
    // This member categorizes the section's contents and semantics.
    ElfSectionType type;
    // Sections support one-bit flags that describe miscellaneous attributes.  If a flag bit is set in sh_flags, the
    // attribute is "on" for the section.  Otherwise, the attribute is "off" or does not apply.  Undefined attributes
    // are set to zero.
    //
    // SHF_WRITE
    //         This section contains data that should be writable during process execution.
    // SHF_ALLOC
    //         This section occupies memory during process execution.  Some control sections do not reside in the memory
    //         image of an object file.  This attribute is off for those sections.
    // SHF_EXECINSTR
    //         This section contains executable machine instructions.
    // SHF_MASKPROC
    //         All bits included in this mask are reserved for processor-specific semantics.
    u32 flags;
    // If this section appears in the memory image of a process, this member holds the address at which the section's
    // first byte should reside. Otherwise, the member contains zero.
    u32 addr;
    // This member's value holds the byte offset from the beginning of the file to the first byte in the section. One
    // section type, SHT_NOBITS, occupies no space in the file, and its sh_offset member locates the conceptual
    // placement in the file.
    u32 offset;
    // This member holds the section's size in bytes. Unless the section type is SHT_NOBITS, the section occupies
    // sh_size bytes in the file. A section of type SHT_NOBITS may have a nonzero size, but it occupies no space in the
    // file.
    u32 size;
    // This member holds a section header table index link, whose interpretation depends on the section type.
    u32 link;
    // This member holds extra information, whose interpretation depends on the section type.
    u32 info;
    // Some sections have address alignment constraints. If a section holds a doubleword, the system must ensure
    // doubleword alignment for the entire section.  That is, the value of sh_addr must be congruent to zero, modulo the
    // value of sh_addralign.  Only zero and positive integral powers of two are allowed.  The value 0 or 1 means that
    // the section has no alignment constraints.
    u32 addralign;
    // Some sections hold a table of fixed-sized entries, such as a symbol table.  For such a section, this member gives
    // the size in bytes for each entry.  This member contains zero if the section does not hold a table of fixed-size
    // entries.
    u32 entsize;
};
static_assert(sizeof(ElfSectionHeader32_Packed) == 40);
#pragma pack(pop)

#pragma pack(push, 1)
struct ElfSectionHeader64_Packed {
    u32 name;
    ElfSectionType type;
    // Sections support one-bit flags that describe miscellaneous attributes.  If a flag bit is set in sh_flags, the
    // attribute is "on" for the section.  Otherwise, the attribute is "off" or does not apply.  Undefined attributes
    // are set to zero.
    //
    // SHF_WRITE
    //         This section contains data that should be writable during process execution.
    // SHF_ALLOC
    //         This section occupies memory during process execution.  Some control sections do not reside in the memory
    //         image of an object file.  This attribute is off for those sections.
    // SHF_EXECINSTR
    //         This section contains executable machine instructions.
    // SHF_MASKPROC
    //         All bits included in this mask are reserved for processor-specific semantics.
    u64 flags;
    // If this section appears in the memory image of a process, this member holds the address at which the section's
    // first byte should reside. Otherwise, the member contains zero.
    u64 addr;
    // This member's value holds the byte offset from the beginning of the file to the first byte in the section. One
    // section type, SHT_NOBITS, occupies no space in the file, and its sh_offset member locates the conceptual
    // placement in the file.
    u64 offset;
    // This member holds the section's size in bytes. Unless the section type is SHT_NOBITS, the section occupies
    // sh_size bytes in the file. A section of type SHT_NOBITS may have a nonzero size, but it occupies no space in the
    // file.
    u64 size;
    // This member holds a section header table index link, whose interpretation depends on the section type.
    u32 link;
    // This member holds extra information, whose interpretation depends on the section type.
    u32 info;
    // Some sections have address alignment constraints. If a section holds a doubleword, the system must ensure
    // doubleword alignment for the entire section.  That is, the value of sh_addr must be congruent to zero, modulo the
    // value of sh_addralign.  Only zero and positive integral powers of two are allowed.  The value 0 or 1 means that
    // the section has no alignment constraints.
    u64 addralign;
    // Some sections hold a table of fixed-sized entries, such as a symbol table.  For such a section, this member gives
    // the size in bytes for each entry.  This member contains zero if the section does not hold a table of fixed-size
    // entries.
    u64 entsize;
};
static_assert(sizeof(ElfSectionHeader64_Packed) == 64);
#pragma pack(pop)

} // namespace elf
