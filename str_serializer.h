#pragma once

#include "elf_types.h"

#include <types.h>

#include <string>

namespace dbg
{

using namespace coretypes;

using ElfClass           = elf::ElfClass;
using ElfEncoding        = elf::ElfEncoding;
using ElfOSAbi           = elf::ElfOSAbi;
using ElfType            = elf::ElfType;
using ElfHeader32_Packed = elf::ElfHeader32_Packed;
using ElfHeader64_Packed = elf::ElfHeader64_Packed;

const char* Str(ElfClass v);
const char* Str(ElfEncoding v);
const char* Str(ElfOSAbi v);
const char* Str(ElfType v);

std::string Str(const ElfHeader32_Packed& v);
std::string Str(const ElfHeader64_Packed& v);

} // namespace dbg
