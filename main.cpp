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
    std::vector<u8> out;
    if (auto res = core::fs::ReadFileFull("main.cpp", O_RDONLY, 0, out); res.IsErr()) {
        std::cout << "Error: " << res.msg << std::endl;
        return 1;
    }

    std::cout << "Read " << out.size() << " bytes" << '\n';
    std::string str (out.begin(), out.end());
    std::cout << "Content: " << str << std::endl;
    return 0;
}
