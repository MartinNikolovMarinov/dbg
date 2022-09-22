#pragma once

#include <core.h>
#include <fmt/core.h>

#include <elf.h>
#include <string>
#include <type_traits>

namespace dbg
{

using namespace coretypes;

template <typename TInt>
constexpr std::string IntToHexStr(TInt v, size_t hexLen = sizeof(TInt)<<1) {
    static_assert(std::is_integral_v<TInt>, "integral type required");
    return fmt::format("0x{:0{}x}", v, hexLen);
}

std::string Str(const Elf32_Ehdr& v);
std::string Str(const Elf64_Ehdr& v);

std::string Str(const Elf32_Phdr& v);
std::string Str(const Elf64_Phdr& v);

std::string Str(const Elf32_Shdr& v);
std::string Str(const Elf64_Shdr& v);

} // namespace dbg
