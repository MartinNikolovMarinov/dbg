#include <core.h>
#include <core_error.h>
#include <io.h>
#include <plt.h>
#include <fs.h>

#include <algorithm>
#include <vector>
#include <iostream>
#include <optional>
#include <errno.h>
#include <cstring>

#include <elf.h>

#include <fcntl.h>

using namespace coretypes;

/**
 * NOTE: This information is taken from the linux manuam elf(5) entry.
 *
 * ELF - Executable and Linkable Format (ELF) files. The ELF header is always at offset zero of the file. The program
 * header table and the section header table's offset in the file are defined in the ELF header.  The two tables
 * describe the rest of the particularities of the file.
*/

i32 main(i32, const char**, const char**) {
    auto res = core::fs::OpenFile("main.cpp", O_RDONLY, 0);
    if (res.IsErr()) {
        std::cout << res.err.msg << std::endl;
        return 1;
    }

    auto file = std::move(res.val);
    constexpr u64 SIZE = 180;

    u8 buf[SIZE];
    auto readRes = core::io::Read<core::fs::File, core::fs::File::ReadResponse>(file, buf, SIZE);
    if (core::io::IsErr(readRes)) {
        std::cout << core::io::Err(readRes).msg << std::endl;
        return 1;
    }
    buf[SIZE - 1] = '\0';

    std::cout << buf << std::endl;
    std::cout << core::io::N(readRes) << std::endl;

    if (auto res = core::io::Close<core::fs::File, core::fs::File::CloseResponse>(file); res.IsErr()) {
        std::cout << res.Err().msg << std::endl;
        return 1;
    }

    return 0;
}
