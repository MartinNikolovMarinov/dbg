#include "elf_types.h"
#include "elf_parser.h"
#include "str_serializer.h"
#include "init.h"

#include <fs.h>

#include <iostream>
#include <fcntl.h>

using namespace coretypes;

const char* helloWorldBinPath = PATH_TO_DATA "/bin/hello_world";

i32 main(i32, const char**, const char**) {
    dbg::Init();

    elf::ByteBuff fileBytes;
    if (auto res = core::fs::ReadFileFull(helloWorldBinPath, O_RDONLY, 0, fileBytes); res.IsErr()) {
        std::cout << "Error: " << res.Err() << std::endl;
        return 1;
    }

    elf::ElfParser p(std::move(fileBytes));
    if (auto err = p.Parse(); err.IsErr()) {
        std::cout << "Error: " << err.Err() << std::endl;
        return 1;
    }

    // Parsing the ELF header:
    auto header = p.GetElf64Header();
    std::cout << dbg::Str(*header) << std::endl;

    // Parsing the program headers:
    for (i64 i = 0; i < p.ProgHeaderCount(); i++) {
        auto pHeader = p.GetElf64ProgHeader(i);
        std::cout << dbg::Str(*pHeader) << std::endl;
    }

    // Parsing the section headers:
    for (i64 i = 0; i < p.SectionHeaderCount(); i++) {
        auto sHeader = p.GetElf64SectionHeader(i);
        // std::cout << dbg::Str(*sHeader) << std::endl;
        std::cout << sHeader->addr << std::endl;
    }

    return 0;
}
