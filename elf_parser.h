#pragma once

#include "elf_section.h"

#include <core.h>
#include <core_error.h>

#include <elf.h>
#include <fmt/core.h>

#include <vector>

namespace elf
{

// TODO: I am not taking into account the endianeds of the ELF file. Does this matter for x86 machines and do I care ?

using namespace coretypes;

using ElfByteBuff = std::vector<u8>;
using ElfError    = core::error::ErrorInt;

static constexpr i32 ElfFileIsShortErr                = 1;
static constexpr i32 ElfHeaderInvalidMagicErr         = 2;
static constexpr i32 ElfHeaderInvalidClassErr         = 3;
static constexpr i32 ElfHeaderIsNot32BitErr           = 4;
static constexpr i32 ElfHeaderIsNot64BitErr           = 5;
static constexpr i32 ElfInvalidProgramHeaderOffsetErr = 6;
static constexpr i32 ElfInvalidProgramHeaderSizeErr   = 7;
static constexpr i32 ElfInvalidSectionHeaderSizeErr   = 8;

template<i32 TElfByteSize = 64>
struct ElfParser {
    static_assert(TElfByteSize == 32 || TElfByteSize == 64, "TElfByteSize must be 32 or 64");

    using ElfEhdr  = std::conditional_t<TElfByteSize == 32, Elf32_Ehdr, Elf64_Ehdr>;
    using ElfPhdr  = std::conditional_t<TElfByteSize == 32, Elf32_Phdr, Elf64_Phdr>;
    using ElfShdr  = std::conditional_t<TElfByteSize == 32, Elf32_Shdr, Elf64_Shdr>;

    ElfParser(ElfByteBuff&& bytes) : m_bytes(std::move(bytes)), m_startEhdr(nullptr) {}

    ElfError Parse() {
        if (m_bytes.size() < sizeof(ElfEhdr)) return ElfFileIsShortErr;

        m_cur = m_bytes.data();
        if (auto err = ParseElfHeader(); err.IsErr()) return err.Err();
        if (auto err = ParseElfProgHeader(); err.IsErr()) return err.Err();
        if (auto err = ParseElfSectionHeader(); err.IsErr()) return err.Err();

        m_cur = nullptr;
        return ElfError::Ok;
    }

    const u8* Bytes() { return m_bytes.data(); }

    const ElfEhdr* GetElfHeader() const {
        Assert(m_startEhdr != nullptr, "Elf header not parsed");
        auto ret = reinterpret_cast<const ElfEhdr*>(m_startEhdr);
        Assert(ret != nullptr, "Implementation error");
        return ret;
    }

    u64 ProgHeaderSize()  const { return GetElfHeader()->e_phentsize; }
    u64 ProgHeaderCount() const { return GetElfHeader()->e_phnum; }

    const ElfPhdr* GetElfProgHeader(i32 idx) const {
        Assert(m_startPhdr != nullptr, "Elf program header not parsed");
        auto ehdr = GetElfHeader();
        if (idx >= ehdr->e_phnum) return nullptr;
        ElfPhdr* ret = reinterpret_cast<ElfPhdr*>(m_startPhdr + (idx * ProgHeaderSize()));
        return ret;
    }

    u64 SectionHeaderSize()  const { return GetElfHeader()->e_shentsize; }
    u64 SectionHeaderCount() const { return GetElfHeader()->e_shnum; }

    const ElfShdr* GetElfSectionHeader(i32 idx) const {
        Assert(m_startShdr != nullptr, "Elf section header not parsed");
        if (idx == SHN_UNDEF) return nullptr; // section is irrelevant/meaningless according to the spec.
        auto ehdr = GetElfHeader();
        if (idx >= ehdr->e_shnum) return nullptr;
        ElfShdr* ret = reinterpret_cast<ElfShdr*>(m_startShdr + (idx * SectionHeaderSize()));
        return ret;
    }

    const ElfShdr* GetElfSectionHeader(SectionType type) const {
        const char* name = SectionTypeToCtpr(type);
        if (name == nullptr) return nullptr;
        for (u64 i = 0; i < SectionHeaderCount(); i++) {
            if (const ElfShdr* shdr = GetElfSectionHeader(i); shdr != nullptr) {
                const char* shdrName = LookupStringTable(shdr->sh_name);
                if (core::CptrCmp(shdrName, name) == 0) {
                    return shdr;
                }
            }
        }
        return nullptr;
    }

    const char* LookupStringTable(i64 offset) const {
        // TODO: check if offset goes past the table size !
        Assert(m_startShdr != nullptr, "Elf section header not parsed");
        auto ehdr = GetElfHeader();
        auto shdr = GetElfSectionHeader(ehdr->e_shstrndx);
        if (shdr == nullptr) return nullptr;
        return reinterpret_cast<const char*>(m_startEhdr + shdr->sh_offset + offset);
    }

    const char* GetSectionName(i32 sectionIdx) const {
        auto shdr = GetElfSectionHeader(sectionIdx);
        if (shdr == nullptr) return nullptr;
        auto ret = LookupStringTable(shdr->sh_name);
        return ret;
    }

private:
    ElfByteBuff m_bytes;

    u8* m_cur;
    u8* m_startEhdr;
    u8* m_startPhdr;
    u8* m_startShdr;

    ElfError ParseElfHeader() {
        // Some PROTOCOL edge cases are not handled:
        // TODO: e_phoff can be zero if the file has no program header !
        // TODO: e_shoff can be zero if the file has no section header !
        // TODO: e_phnum
        // If the number of entries in the program header table is
        // larger than or equal to PN_XNUM (0xffff), this member
        // holds PN_XNUM (0xffff) and the real number of entries in
        // the program header table is held in the sh_info member of
        // the initial entry in section header table.  Otherwise, the
        // sh_info member of the initial entry contains the value
        // zero.
        // TODO: e_shnum
        // If the number of entries in the section header table is
        // larger than or equal to SHN_LORESERVE (0xff00), e_shnum
        // holds the value zero and the real number of entries in the
        // section header table is held in the sh_size member of the
        // initial entry in section header table.  Otherwise, the
        // sh_size member of the initial entry in the section header
        // table holds the value zero.
        // TODO: e_shstrndx
        // If the index of section name string table section is
        // larger than or equal to SHN_LORESERVE (0xff00), this
        // member holds SHN_XINDEX (0xffff) and the real index of the
        // section name string table section is held in the sh_link
        // member of the initial entry in section header table.
        // Otherwise, the sh_link member of the initial entry in
        // section header table contains the value zero.


        if (auto err = CheckMagic(m_cur); err.IsErr()) {
            return err.Err();
        }
        if (*(m_cur + 4) != ELFCLASS32 && *(m_cur + 4) != ELFCLASS64) {
            return ElfHeaderInvalidClassErr;
        }

        if constexpr (TElfByteSize == 32) {
            if (*(m_cur + 4) != ELFCLASS32) return ElfHeaderIsNot32BitErr;
        } else {
            if (*(m_cur + 4) != ELFCLASS64) return ElfHeaderIsNot64BitErr;
        }

        // set the elf header ptr
        m_startEhdr = m_cur;
        auto ehdr = GetElfHeader();
        if (ehdr->e_ehsize != sizeof(ElfEhdr)) {
            // ElfClass field lied to me !
            return ElfHeaderIsNot64BitErr;
        }
        m_cur += sizeof(ElfEhdr);
        return ElfError::Ok;
    }

    ElfError ParseElfProgHeader() {
        auto ehdr = GetElfHeader();
        if (ehdr->e_phentsize != sizeof(ElfPhdr)) {
            return ElfInvalidProgramHeaderSizeErr;
        }
        if ((m_cur - m_startEhdr) != i64(ehdr->e_phoff)) {
            return ElfInvalidProgramHeaderOffsetErr;
        }
        m_startPhdr = m_cur;
        m_cur += ehdr->e_phentsize;
        return ElfError::Ok;
    }

    ElfError ParseElfSectionHeader() {
        auto ehdr = GetElfHeader();
        if (ehdr->e_shentsize != sizeof(ElfShdr)) {
            return ElfInvalidSectionHeaderSizeErr;
        }
        m_startShdr = m_startEhdr + ehdr->e_shoff;
        return ElfError::Ok;
    }

    ElfError CheckMagic(u8* ptr) {
        bool ok = *(ptr + 0) == ELFMAG0 &&
                  *(ptr + 1) == ELFMAG1 &&
                  *(ptr + 2) == ELFMAG2 &&
                  *(ptr + 3) == ELFMAG3;
        return ok ? ElfError::Ok : ElfHeaderInvalidMagicErr;
    }
};

} // namespace elf
