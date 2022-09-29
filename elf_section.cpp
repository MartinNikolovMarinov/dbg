#include "elf_section.h"

namespace elf
{

namespace
{
// Section Type lookup table
static struct{ SectionType type; const char* name; } sectionTypeLT[] = {
    { SectionType::UNKNOWN, "unknown" },
    { SectionType::DWARF_INFO, ".debug_info" },
};
}

const char* SectionTypeToCtpr(SectionType type) {
    if (i32(type) < 0 || i32(type) >= i32(SectionType::COUNT)) return nullptr;
    return sectionTypeLT[i32(type)].name;
}

} // namespace elf
