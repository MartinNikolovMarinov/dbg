#include "str_serializer.h"

#include <iomanip>
#include <sstream>

namespace dbg
{

using namespace coretypes;

const char* Str(ElfClass v) {
    switch (v) {
        case ElfClass::CLASSNONE: return "CLASS_NONE";
        case ElfClass::ELF32:     return "CLASS_ELF32";
        case ElfClass::ELF64:     return "CLASS_ELF64";
        default:                  return "CLASS_NONE";
    }
}

const char* Str(ElfEncoding v) {
    switch (v) {
        case ElfEncoding::DATANONE:     return "ENCODING_NONE";
        case ElfEncoding::LITTLEENDIAN: return "ENCODING_LITTLEENDIAN";
        case ElfEncoding::BIGENDIAN:    return "ENCODING_BIGENDIAN";
        default:                        return "ENCODING_NONE";
    }
}

const char* Str(ElfOSAbi v) {
    switch (v) {
        case ElfOSAbi::UNIX:         return "OSABI_UNIX";
        case ElfOSAbi::HPUX:         return "OSABI_HPUX";
        case ElfOSAbi::NETBSD:       return "OSABI_NETBSD";
        case ElfOSAbi::LINUX:        return "OSABI_LINUX";
        case ElfOSAbi::SOLARIS:      return "OSABI_SOLARIS";
        case ElfOSAbi::AIX:          return "OSABI_AIX";
        case ElfOSAbi::IRIX:         return "OSABI_IRIX";
        case ElfOSAbi::FREEBSD:      return "OSABI_FREEBSD";
        case ElfOSAbi::TRU64:        return "OSABI_TRU64";
        case ElfOSAbi::MODESTO:      return "OSABI_MODESTO";
        case ElfOSAbi::OPENBSD:      return "OSABI_OPENBSD";
        case ElfOSAbi::ARM_AEABI:    return "OSABI_ARM_AEABI";
        case ElfOSAbi::ARM:          return "OSABI_ARM";
        case ElfOSAbi::STANDALONE:   return "OSABI_STANDALONE";
        default:                     return "OSABI_UNIX";
    }
}

const char* Str(ElfType v) {
    switch (v) {
        case ElfType::ETNONE: return "TYPE_NONE";
        case ElfType::ETREL:  return "TYPE_REL";
        case ElfType::ETEXEC: return "TYPE_EXEC";
        case ElfType::ETDYN:  return "TYPE_DYN";
        case ElfType::ETCORE: return "TYPE_CORE";
        default:              return "TYPE_NONE";
    }
}

// FIXME: Fix this slow dog shit. Code below this comment is just embarrassing.


template <typename I> std::string IntToHex(I w, size_t hex_len = sizeof(I)<<1) {
    static const char* digits = "0123456789ABCDEF";
    std::string rc(hex_len, '0');
    for (size_t i=0, j=(hex_len-1)*4 ; i<hex_len; ++i,j-=4) {
        rc[i] = digits[(w>>j) & 0x0f];
    }
    return rc;
}

namespace
{

template <typename THeader>
void CommonElfHeaderToStr(const THeader& v, std::string& out) {
    // Write the ident part of the header:
    out += "  Class: " + std::string(Str(v.elfclass)) + "\n";
    out += "  Encoding: " + std::string(Str(v.encoding)) + "\n";
    out += "  Version: " + std::to_string(v.version) + "\n";
    out += "  OS/ABI: " + std::string(Str(v.os)) + "\n";
    out += "  ABI Version: " + std::to_string(v.abi) + "\n";

    // Write the rest of the header:
    out += "  Type: " + std::string(Str(v.type)) + "\n";
    out += "  Machine: " + std::to_string(v.machine) + "\n";
    out += "  Version: " + std::to_string(v.version) + "\n";
    out += "  Entry point address: " + std::to_string(v.entry) + "\n";
    out += "  Start of program headers: " + std::to_string(v.phoff) + "\n";
    out += "  Start of section headers: " + std::to_string(v.shoff) + "\n";
    out += "  Flags: " + std::to_string(v.flags) + "\n";
    out += "  Size of this header: " + std::to_string(v.ehsize) + "\n";
    out += "  Size of program headers: " + std::to_string(v.phentsize) + "\n";
    out += "  Number of program headers: " + std::to_string(v.phnum) + "\n";
    out += "  Size of section headers: " + std::to_string(v.shentsize) + "\n";
    out += "  Number of section headers: " + std::to_string(v.shnum) + "\n";
    out += "  Section header string table index: " + std::to_string(v.shtrndx) + "\n";
}

}; // namespace

std::string Str(const ElfHeader32_Packed& v) {
    std::string ret;
    // Write the entre header byte by byte in hex:
    const u8* bytes = reinterpret_cast<const u8*>(&v);
    constexpr u64 hSizeInBytes = sizeof(ElfHeader32_Packed);
    ret += "ElfHeader32_Packed {\n";
    ret += "  Magic: ";
    for (u64 i = 0; i < hSizeInBytes; ++i) {
        ret += IntToHex(bytes[i], sizeof(u8) * 2);
        ret += " ";
    }
    ret += "\n";
    CommonElfHeaderToStr(v, ret);
    ret += "}\n";
    return ret;
}

std::string Str(const ElfHeader64_Packed& v) {
    std::string ret;
    // Write the entre header byte by byte in hex:
    const u8* bytes = reinterpret_cast<const u8*>(&v);
    constexpr u64 hSizeInBytes = sizeof(ElfHeader64_Packed);
    ret += "ElfHeader64_Packed {\n";
    ret += "  Magic: ";
    for (u64 i = 0; i < hSizeInBytes; ++i) {
        ret += IntToHex(bytes[i], sizeof(u8) * 2);
        ret += " ";
    }
    ret += "\n";
    CommonElfHeaderToStr(v, ret);
    ret += "}\n";
    return ret;
}

} // namespace dbg
