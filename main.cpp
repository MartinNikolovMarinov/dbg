#include "elf_section.h"
#include "elf_parser.h"
#include "str_serializer.h"
#include "init.h"

#include <core.h>
#include <fs.h>

#include <iostream>
#include <fcntl.h>

#include <core_error.h>

using namespace coretypes;

const char* helloWorldBinPath = PATH_TO_DATA "/bin/hello_world";

u32 DecodeULEB128(const u8* data, u32& idx) {
    u32 ret = 0, shift = 0;
    u8 byte = 0;
    do {
        byte = data[idx++];
        ret |= (byte & 0x7f) << shift;
        shift += 7;
    } while (byte & 0x80);
    return ret;
}

// NOTE: On the DWARF format.
//
// The Debugging Information Entry (DIE)
// ------------------------------------
// DWARF uses a series of debugging information entries (DIEs) to define a low-level representation of a source program.
// Each debugging information entry consists of an identifying tag and a series of attributes. An entry, or group of
// entries together, provide a description of a corresponding entity in the source program. The tag specifies the class
// to which an entry belongs and the attributes define the specific characteristics of the entry.
//
// DIEs can be split into two general types. Those that describe data including data types and those that describe
// functions and other executable code.

struct CompilationUnit {
    u32 len;
    u16 version;
    u8 unitType;
    u32 abbrevOffset;
    u8 addrSize;

    bool IsValid() const {
        bool ret = (len > 0) && (version > 0) && (unitType > 0) && (addrSize > 0);
        return ret;
    }

    std::string Str() {
        std::string ret = fmt::format("CompilationUnit: len: {0:d} ({0:#x}), version: {1:d}, unitType: {2:d}, abbrevOffset: {3:#x}, addrSize: {4:d}",
                                      len, version, unitType, abbrevOffset, addrSize);
        return ret;
    }
};

i32 main(i32, const char**, const char**) {
    dbg::Init();

    elf::ElfByteBuff fileBytes;
    if (auto res = core::fs::ReadFileFull(helloWorldBinPath, O_RDONLY, 0, fileBytes); res.IsErr()) {
        std::cout << "Error: " << res.Err() << std::endl;
        return 1;
    }

    elf::ElfParser p(std::move(fileBytes));
    if (auto err = p.Parse(); err.IsErr()) {
        std::cout << "Error: " << err.Err() << std::endl;
        return 1;
    }

    auto infoSection = p.GetElfSectionHeader(elf::SectionType::DWARF_INFO);
    Assert(infoSection != nullptr);
    const u8* debugStart = &(p.Bytes()[infoSection->sh_offset]);

    // Compilation unit header:
    CompilationUnit cu;
    // NOTE: that these values are not uleb128 encoded!
    cu.len = *(u32*)debugStart;
    debugStart += sizeof(u32);
    cu.version = *(u16*)debugStart;
    debugStart += sizeof(u16);
    cu.unitType = *(u8*)debugStart;
    debugStart += sizeof(u8);
    cu.addrSize = *(u8*)debugStart;
    debugStart += sizeof(u8);
    cu.abbrevOffset = *(u32*)debugStart;
    debugStart += sizeof(u32);
    Assert(cu.IsValid());
    fmt::print("{}\n", cu.Str());

    u32 offset = 0;
    u32 abbrevIdx = DecodeULEB128(debugStart, offset);

    auto addrevSection = p.GetElfSectionHeader(elf::SectionType::DWARF_ABBREV);
    Assert(addrevSection != nullptr);
    const u8* abbrevStart = &(p.Bytes()[addrevSection->sh_offset + u64(cu.abbrevOffset)]);

    return 0;
}
