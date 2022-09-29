#pragma once

#include <core.h>

namespace elf
{

using namespace coretypes;

enum struct SectionType : i32 {
    UNKNOWN,
    DWARF_INFO,

    COUNT
};

const char* SectionTypeToCtpr(SectionType type);

}
