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

// Debugging information entry (DIE)
struct Die {
    u32 offset;
    u32 tag;
    u32 abbrevCode;
    u32 children;
    u32 attributes;
};

u32 DecodeULEB128(const u8* data, u32& idx) {
    u32 ret = 0;
    u32 shift = 0;
    u8 byte = 0;
    do {
        byte = data[idx++];
        ret |= (byte & 0x7f) << shift;
        shift += 7;
    } while (byte & 0x80);
    return ret;
}

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

    return 0;
}
