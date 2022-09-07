#include "str_serializer.h"

#include <iomanip>
#include <sstream>
#include <type_traits>

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

// FIXME: Code below this comment is just embarrassing.
//        I actually need a good formatting library. Somthing like fmtlib, or write my own.

namespace
{

std::string& LTrimStr(std::string& str, std::string_view chars) {
    str.erase(0, str.find_first_not_of(chars));
    return str;
}

std::string& RTrimStr(std::string& str, std::string_view chars) {
    str.erase(str.find_last_not_of(chars) + 1);
    return str;
}

std::string& TrimStr(std::string& str, std::string_view chars) {
    return LTrimStr(RTrimStr(str, chars), chars);
}

template <typename TInt> std::string IntToHexStr(TInt w, size_t hex_len = sizeof(TInt)<<1) {
    static_assert(std::is_integral_v<TInt>, "integral type required");
    if (w == 0) return "0x0";
    std::string res(hex_len, '0'); // 1 copy
    core::IntToHex(w, res.data(), hex_len);
    res = LTrimStr(res, "0"); // 2 copies
    return "0x" + res; // 3 copies
}

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
    out += "  Version: " + IntToHexStr(v.version) + "\n";
    out += "  Entry point address: " + IntToHexStr(v.entry) + "\n";
    out += "  Start of program headers: " + std::to_string(v.phoff) + "\n";
    out += "  Start of section headers: " + std::to_string(v.shoff) + "\n";
    out += "  Flags: " + IntToHexStr(v.flags) + "\n";
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
        ret += IntToHexStr(bytes[i], sizeof(u8) * 2);
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
        ret += IntToHexStr(bytes[i], sizeof(u8) * 2);
        ret += " ";
    }
    ret += "\n";
    CommonElfHeaderToStr(v, ret);
    ret += "}\n";
    return ret;
}

const char* Str(ElfProgHeaderType v) {
    switch (v) {
        case ElfProgHeaderType::NUL:        return "NULL";
        case ElfProgHeaderType::LOAD:       return "LOAD";
        case ElfProgHeaderType::DYNAMIC:    return "DYNAMIC";
        case ElfProgHeaderType::INTERP:     return "INTERP";
        case ElfProgHeaderType::NOTE :      return "NOTE";
        case ElfProgHeaderType::SHLIB:      return "SHLIB";
        case ElfProgHeaderType::PHDR:       return "PHDR";
        case ElfProgHeaderType::TLS:        return "TLS";
        case ElfProgHeaderType::COUNT:      return "UNKNOWN";
        default:                            break;
    }

    if (v >= ElfProgHeaderType::LOOS && v <= ElfProgHeaderType::HIOS)     return "OS SPECIFIC";
    if (v >= ElfProgHeaderType::LOPROC && v <= ElfProgHeaderType::HIPROC) return "PROC SPECIFIC";
    if (v >= ElfProgHeaderType::LOUSER && v <= ElfProgHeaderType::HIUSER) return "USER SPECIFIC";

    return "UNKNOWN";
}

const char* Str(ElfProgHeaderFlags v) {
    switch (v) {
        case ElfProgHeaderFlags::E:   return" E";
        case ElfProgHeaderFlags::W:   return "W";
        case ElfProgHeaderFlags::R:   return "R";
        case ElfProgHeaderFlags::RW:  return "RW";
        case ElfProgHeaderFlags::RE:  return "RE";
        case ElfProgHeaderFlags::WE:  return "WE";
        case ElfProgHeaderFlags::RWE: return "RWE";
        default:                      return "UNKNOWN"; // maybe os specific
    }
}

namespace
{

template<typename THeader>
void CommonElfProgHeaderToStr(const THeader v, std::string& out) {
    out += "  Type: " + std::string(Str(v.type)) + "\n";
    out += "  Offset: " + IntToHexStr(v.offset) + "\n";
    out += "  Virtual address: " + IntToHexStr(v.vaddr) + "\n";
    out += "  Physical address: " + IntToHexStr(v.paddr) + "\n";
    out += "  File size: " + IntToHexStr(v.filesz) + "\n";
    out += "  Memory size: " + IntToHexStr(v.memsz) + "\n";
    out += "  Flags: " + std::string(Str(v.flags)) + "\n";
    out += "  Alignment: " + IntToHexStr(v.align) + "\n";
}

} // namespace

std::string Str(const ElfProgHeader32_Packed& v) {
    std::string ret;
    ret += "ElfProgHeader32_Packed {\n";
    CommonElfProgHeaderToStr(v, ret);
    ret += "}\n";
    return ret;
}

std::string Str(const ElfProgHeader64_Packed& v) {
    std::string ret;
    ret += "ElfProgHeader64_Packed {\n";
    CommonElfProgHeaderToStr(v, ret);
    ret += "}\n";
    return ret;
}

} // namespace dbg
