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

    auto header = p.GetElf64Header();
    std::cout << dbg::Str(header) << std::endl;

    return 0;
}
