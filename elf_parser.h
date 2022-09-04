#pragma once

#include "elf_types.h"

#include <core.h>
#include <core_error.h>

#include <vector>

namespace elf
{

using namespace coretypes;

using ByteBuff = std::vector<u8>;
using BytesIt  = ByteBuff::iterator;

using ElfError           = core::error::ErrorInt;
template<typename T>
using ElfErrorValue      = core::error::ErrorValue<T, ElfError>;
using ElfHeader32_Packed = elf::ElfHeader32_Packed;
using ElfHeader64_Packed = elf::ElfHeader64_Packed;
using ElfClass           = elf::ElfClass;
using ElfEncoding        = elf::ElfEncoding;
using ElfOSAbi           = elf::ElfOSAbi;
using ElfType            = elf::ElfType;

static constexpr i32 ElfHeaderInvalidSize     = 1;
static constexpr i32 ElfHeaderInvalidMagicErr = 2;
static constexpr i32 ElfHeaderInvalidClassErr = 3;
static constexpr i32 ElfHeaderIsNot32BitErr   = 4;
static constexpr i32 ElfHeaderIsNot64BitErr   = 5;

struct ElfParser {
    ElfParser(ByteBuff&& bytes);

    ElfError Parse();

    const ElfHeader32_Packed& GetElf32Header() const;
    const ElfHeader64_Packed& GetElf64Header() const;

private:
    ByteBuff m_bytes;

    // Elf related types:
    BytesIt m_headerIt;
    bool m_is64Bit;

    ElfError ParseElfHeader(BytesIt& it);
    ElfError CheckMagic(BytesIt& it);
};

} // namespace elf