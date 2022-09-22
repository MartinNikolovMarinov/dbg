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

    // Parsing the ELF header:
    auto ehdr = p.GetElfHeader();
    std::cout << dbg::Str(*ehdr) << std::endl;

    // Parsing the program headers:
    for (i64 i = 0; i < i64(p.ProgHeaderCount()); i++) {
        auto phdr = p.GetElfProgHeader(i);
        std::cout << dbg::Str(*phdr) << std::endl;
    }

    // Parsing the section headers:
    for (i64 i = 0; i < i64(p.SectionHeaderCount()); i++) {
        auto shdr = p.GetElfSectionHeader(i);
        auto sectionName = p.GetSectionName(i);
        std::cout << "Section Name: " << sectionName << std::endl;
        std::cout << dbg::Str(*shdr) << std::endl;
    }

    return 0;
}
