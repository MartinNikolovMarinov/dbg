#include "str_serializer.h"

namespace dbg
{
namespace
{

constexpr const char* StrElfClass(u8 v) {
    switch (v) {
        case ELFCLASSNONE: return "ELFCLASSNONE";
        case ELFCLASS32:   return "ELFCLASS32";
        case ELFCLASS64:   return "ELFCLASS64";
        default:           return "UNKNOWN";
    }
}

constexpr const char* StrElfEncoding(u8 v) {
    switch (v) {
        case ELFDATANONE: return "ELFDATANONE";
        case ELFDATA2LSB: return "ELFDATA2LSB";
        case ELFDATA2MSB: return "ELFDATA2MSB";
        default:          return "UNKNOWN";
    }
}

constexpr const char* StrElfOSAbi(u8 v) {
    switch (v) {
        case ELFOSABI_NONE:         return "ELFOSABI_UNIX";
        case ELFOSABI_HPUX:         return "ELFOSABI_HPUX";
        case ELFOSABI_NETBSD:       return "ELFOSABI_NETBSD";
        case ELFOSABI_LINUX:        return "ELFOSABI_LINUX";
        case ELFOSABI_SOLARIS:      return "ELFOSABI_SOLARIS";
        case ELFOSABI_AIX:          return "ELFOSABI_AIX";
        case ELFOSABI_IRIX:         return "ELFOSABI_IRIX";
        case ELFOSABI_FREEBSD:      return "ELFOSABI_FREEBSD";
        case ELFOSABI_TRU64:        return "ELFOSABI_TRU64";
        case ELFOSABI_MODESTO:      return "ELFOSABI_MODESTO";
        case ELFOSABI_OPENBSD:      return "ELFOSABI_OPENBSD";
        case ELFOSABI_ARM_AEABI:    return "ELFOSABI_ARM_AEABI";
        case ELFOSABI_ARM:          return "ELFOSABI_ARM";
        case ELFOSABI_STANDALONE:   return "ELFOSABI_STANDALONE";
        default:                    return "UNKNOWN";
    }
}

constexpr const char* StrElfType(u8 v) {
    switch (v) {
        case ET_NONE: return "ET_NONE";
        case ET_REL:  return "ET_REL";
        case ET_EXEC: return "ET_EXEC";
        case ET_DYN:  return "ET_DYN";
        case ET_CORE: return "ET_CORE";
        default:      return "UNKNOWN";
    }
}

template <typename THeader>
constexpr void CommonElfHeaderToStr(const THeader& v, std::string& out) {
    // Write the ident part of the header:
    out += fmt::format("  Class: {}\n", StrElfClass(v.e_ident[EI_CLASS]));
    out += fmt::format("  Encoding: {}\n", StrElfEncoding(v.e_ident[EI_DATA]));
    out += fmt::format("  Version: {}\n", v.e_ident[EI_VERSION]);
    out += fmt::format("  OS/ABI: {}\n", StrElfOSAbi(v.e_ident[EI_OSABI]));
    out += fmt::format("  ABI Version: {}\n", v.e_ident[EI_ABIVERSION]);

    // Write the rest of the header:
    out += fmt::format("  Type: {}\n", StrElfType(v.e_type));
    out += fmt::format("  Machine: {}\n", v.e_machine);
    out += fmt::format("  Version: {}\n", IntToHexStr(v.e_version));
    out += fmt::format("  Entry point address: {}\n", IntToHexStr(v.e_entry));
    out += fmt::format("  Start of program headers: {}\n", v.e_phoff);
    out += fmt::format("  Start of section headers: {}\n", v.e_shoff);
    out += fmt::format("  Flags: {}\n", IntToHexStr(v.e_flags));
    out += fmt::format("  Size of this header: {}\n", v.e_ehsize);
    out += fmt::format("  Size of program headers: {}\n", v.e_phentsize);
    out += fmt::format("  Number of program headers: {}\n", v.e_phnum);
    out += fmt::format("  Size of section headers: {}\n", v.e_shentsize);
    out += fmt::format("  Number of section headers: {}\n", v.e_shnum);
    out += fmt::format("  Section header string table index: {}\n", v.e_shstrndx);
}

}; // namespace

std::string Str(const Elf32_Ehdr& v) {
    std::string ret;
    // Write the entre header byte by byte in hex:
    const u8* bytes = reinterpret_cast<const u8*>(&v);
    ret += "Elf32_Ehdr {\n";
    ret += "  Magic: ";
    for (u64 i = 0; i < EI_NIDENT; ++i) {
        ret += IntToHexStr(bytes[i], sizeof(u8) * 2);
        ret += " ";
    }
    ret += "\n";
    CommonElfHeaderToStr(v, ret);
    ret += "}\n";
    return ret;
}

std::string Str(const Elf64_Ehdr& v) {
    std::string ret;
    // Write the entre header byte by byte in hex:
    const u8* bytes = reinterpret_cast<const u8*>(&v);
    ret += "Elf64_Ehdr {\n";
    ret += "  Magic: ";
    for (u64 i = 0; i < EI_NIDENT; ++i) {
        ret += IntToHexStr(bytes[i], sizeof(u8) * 2);
        ret += " ";
    }
    ret += "\n";
    CommonElfHeaderToStr(v, ret);
    ret += "}\n";
    return ret;
}

namespace
{

constexpr const char* StrProgHeaderType(u32 v) {
    switch (v) {
        case PT_NULL:       return "NULL";
        case PT_LOAD:       return "LOAD";
        case PT_DYNAMIC:    return "DYNAMIC";
        case PT_INTERP:     return "INTERP";
        case PT_NOTE :      return "NOTE";
        case PT_SHLIB:      return "SHLIB";
        case PT_PHDR:       return "PHDR";
        case PT_TLS:        return "TLS";
    }

    if (v >= PT_LOOS && v <= PT_HIOS) {
        switch (v) {
            case PT_GNU_EH_FRAME: return "PT_GNU_EH_FRAME";
            case PT_GNU_STACK:    return "PT_GNU_STACK";
            case PT_GNU_RELRO:    return "PT_GNU_RELRO";
            case PT_GNU_PROPERTY: return "PT_GNU_PROPERTY";
        }
        return "OS SPECIFIC UNKNOWN";
    }
    if (v >= PT_LOPROC && v <= PT_HIPROC) return "PROC SPECIFIC";

    return "UNKNOWN";
}

constexpr const char* StrProgHeaderFlags(u64 v) {
    if ((v & PF_W) && (v & PF_R) && (v & PF_X)) return "RWE";
    if ((v & PF_W) && (v & PF_R))               return "RW";
    if ((v & PF_R) && (v & PF_X))               return "RE";
    if ((v & PF_W) && (v & PF_X))               return "WE";
    if (v & PF_R)                               return "R";
    if (v & PF_W)                               return "W";
    if (v & PF_X)                               return "E";
    return "UNKNOWN"; // maybe os specific
}

template<typename THeader>
constexpr void CommonElfProgHeaderToStr(const THeader v, std::string& out) {
    out += fmt::format("  Type: {}\n", StrProgHeaderType(v.p_type));
    out += fmt::format("  Offset: {}\n", IntToHexStr(v.p_offset));
    out += fmt::format("  Virtual address: {}\n", IntToHexStr(v.p_vaddr));
    out += fmt::format("  Physical address: {}\n", IntToHexStr(v.p_paddr));
    out += fmt::format("  File size: {}\n", IntToHexStr(v.p_filesz));
    out += fmt::format("  Memory size: {}\n", IntToHexStr(v.p_memsz));
    out += fmt::format("  Flags: {}\n", StrProgHeaderFlags(v.p_flags));
    out += fmt::format("  Alignment: {}\n", IntToHexStr(v.p_align));
}

} // namespace

std::string Str(const Elf32_Phdr& v) {
    std::string ret;
    ret += "Elf32_Phdr {\n";
    CommonElfProgHeaderToStr(v, ret);
    ret += "}\n";
    return ret;
}

std::string Str(const Elf64_Phdr& v) {
    std::string ret;
    ret += "Elf64_Phdr {\n";
    CommonElfProgHeaderToStr(v, ret);
    ret += "}\n";
    return ret;
}

namespace {

constexpr const char* StrElfSectionHeaderType(u32 v) {
     switch (v) {
        case SHT_NULL:          return "SHT_NULL";
        case SHT_PROGBITS:      return "SHT_PROGBITS";
        case SHT_SYMTAB:        return "SHT_SYMTAB";
        case SHT_STRTAB:        return "SHT_STRTAB";
        case SHT_RELA:          return "SHT_RELA";
        case SHT_HASH:          return "SHT_HASH";
        case SHT_DYNAMIC:       return "SHT_DYNAMIC";
        case SHT_NOTE :         return "SHT_NOTE";
        case SHT_NOBITS:        return "SHT_NOBITS";
        case SHT_REL:           return "SHT_REL";
        case SHT_SHLIB:         return "SHT_SHLIB";
        case SHT_DYNSYM:        return "SHT_DYNSYM";
        case SHT_INIT_ARRAY:    return "SHT_INIT_ARRAY";
        case SHT_FINI_ARRAY:    return "SHT_FINI_ARRAY";
        case SHT_PREINIT_ARRAY: return "SHT_PREINIT_ARRAY";
        case SHT_GROUP:         return "SHT_GROUP";
        case SHT_SYMTAB_SHNDX:  return "SHT_SYMTAB_SHNDX";
    }

    if (v >= SHT_LOOS && v <= SHT_HIOS) {
        switch (v) {
            case SHT_GNU_ATTRIBUTES: return "SHT_GNU_ATTRIBUTES";
            case SHT_GNU_HASH:       return "SHT_GNU_HASH";
            case SHT_CHECKSUM:       return "SHT_CHECKSUM";
            case SHT_LOSUNW:         return "SHT_LOSUNW";
            case SHT_GNU_LIBLIST:    return "SHT_GNU_LIBLIST";
            case SHT_GNU_verdef:     return "SHT_GNU_verdef";
            case SHT_GNU_verneed:    return "SHT_GNU_verneed";
            case SHT_GNU_versym:     return "SHT_GNU_versym";
        }
        return "OS SPECIFIC UNKNOWN";
    }
    if (v >= SHT_LOPROC && v <= SHT_HIPROC) return "PROC SPECIFIC";
    if (v >= SHT_LOUSER && v <= SHT_HIUSER) return "USER SPECIFIC";

    return "UNKNOWN";
}

constexpr void StrElfSectionFlags(u64 flags, char out[64]) {
    int i = 0;
    if (flags & SHF_WRITE)            out[i++] = 'W'; // writeable
    if (flags & SHF_ALLOC)            out[i++] = 'A'; // allocated
    if (flags & SHF_EXECINSTR)        out[i++] = 'X'; // executable
    if (flags & SHF_MERGE)            out[i++] = 'M'; // merge
    if (flags & SHF_STRINGS)          out[i++] = 'S'; // strings
    if (flags & SHF_INFO_LINK)        out[i++] = 'I'; // info link
    if (flags & SHF_LINK_ORDER)       out[i++] = 'L'; // link order
    if (flags & SHF_OS_NONCONFORMING) out[i++] = 'O'; // extra os processing required
    if (flags & SHF_GROUP)            out[i++] = 'G'; // group
    if (flags & SHF_TLS)              out[i++] = 'T'; // tls
    if (flags & SHF_COMPRESSED)       out[i++] = 'C'; // compressed
    if (flags & SHF_EXCLUDE)          out[i++] = 'E'; // exclude
    if (flags & SHF_MASKOS)           out[i++] = 'o'; // os specific
    if (flags & SHF_MASKPROC)         out[i++] = 'p'; // processor specific
    out[i] = '\0';
}

template<typename THeader>
constexpr void CommonElfSectionHeaderStr(const THeader v, std::string& out) {
    out += fmt::format("  Name: {}\n", v.sh_name);
    out += fmt::format("  Type: {}\n", StrElfSectionHeaderType(v.sh_type));
    out += fmt::format("  Address: {}\n", IntToHexStr(v.sh_addr));
    out += fmt::format("  Offset: {}\n", IntToHexStr(v.sh_offset));
    out += fmt::format("  Size: {}\n", IntToHexStr(v.sh_size));
    out += fmt::format("  EntSize: {}\n", IntToHexStr(v.sh_entsize));
    char buf[64];
    StrElfSectionFlags(v.sh_flags, buf);
    out += fmt::format("  Flags: {}\n", buf);
    out += fmt::format("  Link: {}\n", v.sh_link);
    out += fmt::format("  Info: {}\n", v.sh_info);
    out += fmt::format("  Align: {}\n", v.sh_addralign);
}

} // namespace

std::string Str(const Elf32_Shdr& v) {
    std::string ret;
    ret += "Elf32_Shdr {\n";
    CommonElfSectionHeaderStr(v, ret);
    ret += "}\n";
    return ret;
}

std::string Str(const Elf64_Shdr& v) {
    std::string ret;
    ret += "Elf64_Shdr {\n";
    CommonElfSectionHeaderStr(v, ret);
    ret += "}\n";
    return ret;
}

} // namespace dbg
