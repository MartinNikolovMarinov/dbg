#include "elf_parser.h"

namespace elf
{

// TODO: I am not taking into account the endianeds of the ELF file. Does this matter for x86 machines and do I care ?

ElfParser::ElfParser(ByteBuff&& bytes)
    : m_bytes(bytes)
    , m_elfHeaderIt(m_bytes.end())
    , m_is64Bit(false)
    , m_progHeaderIt(m_bytes.end())
    , m_progHeaderSize(-1)
    , m_progHeaderCount(-1) {}

ElfError ElfParser::Parse() {
    if (m_bytes.size() < sizeof(ElfHeader64_Packed)) {
        // Assume that the file can't be less than the size of a 64 bit ELF header.
        // Technically it could be just a 32 bit ELF header and no other content,
        // but I don't care about that.
        return ElfFileIsShortErr;
    }

    BytesIt parseIt = m_bytes.begin();
    if (auto err = ParseElfHeader(parseIt); err.IsErr()) {
        return err.Err();
    }
    if (auto err = ParseElfProgHeader(parseIt); err.IsErr()) {
        return err.Err();
    }
    if (auto err = ParseElfSectionHeader(parseIt); err.IsErr()) {
        return err.Err();
    }

    return ElfError::Ok;
}

const ElfHeader32_Packed* ElfParser::GetElf32Header() const {
    Assert(m_elfHeaderIt != m_bytes.end(), "Elf header not parsed");
    Assert(!m_is64Bit, "Elf header is NOT 32 bit");
    ElfHeader32_Packed* ret = reinterpret_cast<ElfHeader32_Packed*>(m_elfHeaderIt.base());
    return ret;
}

const ElfHeader64_Packed* ElfParser::GetElf64Header() const {
    Assert(m_elfHeaderIt != m_bytes.end(), "Elf header not parsed");
    Assert(m_is64Bit, "Elf header is NOT 64 bit");
    ElfHeader64_Packed* ret = reinterpret_cast<ElfHeader64_Packed*>(m_elfHeaderIt.base());
    return ret;
}

const ElfProgHeader32_Packed* ElfParser::GetElf32ProgHeader(i32 idx) const {
    Assert(m_progHeaderIt != m_bytes.end(), "Elf program header not parsed");
    Assert(!m_is64Bit, "Elf program header is NOT 32 bit");
    if (idx >= m_progHeaderCount) return nullptr;
    ElfProgHeader32_Packed* ret = reinterpret_cast<ElfProgHeader32_Packed*>(
        m_progHeaderIt.base() + (idx * m_progHeaderSize));
    return ret;
}

const ElfProgHeader64_Packed* ElfParser::GetElf64ProgHeader(i32 idx) const {
    Assert(m_progHeaderIt != m_bytes.end(), "Elf program header not parsed");
    Assert(m_is64Bit, "Elf program header is NOT 64 bit");
    if (idx >= m_progHeaderCount) return nullptr;
    ElfProgHeader64_Packed* ret = reinterpret_cast<ElfProgHeader64_Packed*>(
        m_progHeaderIt.base() + (idx * m_progHeaderSize));
    return ret;
}

const ElfSectionHeader32_Packed* ElfParser::GetElf32SectionHeader(i32) const {
    Assert(m_sectionHeaderIt != m_bytes.end(), "Elf section header not parsed");
    Assert(!m_is64Bit, "Elf section  header is NOT 32 bit");
    Assert("TODO: Implement");
    return nullptr;
}

const ElfSectionHeader64_Packed* ElfParser::GetElf64SectionHeader(i32 idx) const {
    Assert(m_sectionHeaderIt != m_bytes.end(), "Elf section header not parsed");
    Assert(m_is64Bit, "Elf section header is NOT 64 bit");
    Assert("TODO: Implement. This does not work the same way as the program header!!");
    return nullptr;
}

ElfError ElfParser::ParseElfHeader(BytesIt& it) {
    if (auto err = CheckMagic(it); err.IsErr()) {
        return err.Err();
    }
    if (*(it + 4) == u8(ElfClass::CLASSNONE)) {
        return ElfHeaderInvalidClassErr;
    }

    // set the elf header ptr
    m_elfHeaderIt = it;
    m_is64Bit = (*(it + 4) == u8(ElfClass::ELF64));

    if (m_is64Bit) {
        auto ret = GetElf64Header();
        if (ret->ehsize != sizeof(ElfHeader64_Packed)) {
            // ElfClass field lied to me !
            return ElfHeaderIsNot64BitErr;
        }

        m_progHeaderSize = ret->phentsize;
        m_progHeaderCount = ret->phnum;
        m_progHeaderOffset = ret->phoff;
        m_sectionHeaderOffset = ret->shoff;
        m_sectionHeaderSize = ret->shentsize;
        m_sectionHeaderCount = ret->shnum;
        it += sizeof(ElfHeader64_Packed);
    } else {
        auto ret = GetElf32Header();
        if (ret->ehsize != sizeof(ElfHeader32_Packed)) {
            // ElfClass field lied to me !
            return ElfHeaderIsNot32BitErr;
        }

        m_progHeaderSize = ret->phentsize;
        m_progHeaderCount = ret->phnum;
        m_progHeaderOffset = ret->phoff;
        m_sectionHeaderOffset = ret->shoff;
        m_sectionHeaderSize = ret->shentsize;
        m_sectionHeaderCount = ret->shnum;
        it += sizeof(ElfHeader64_Packed);
    }

    return ElfError::Ok;
}

ElfError ElfParser::ParseElfProgHeader(BytesIt& it) {
    if (m_progHeaderOffset != u64(it.base() - m_elfHeaderIt.base())) {
        return ElfInvalidProgramHeaderOffsetErr;
    }

    m_progHeaderIt = it;
    it += m_progHeaderSize * m_progHeaderCount;
    return ElfError::Ok;
}

ElfError ElfParser::ParseElfSectionHeader(BytesIt&) {
    m_sectionHeaderIt = m_elfHeaderIt + m_sectionHeaderOffset;
    return ElfError::Ok;
}

ElfError ElfParser::CheckMagic(BytesIt& it) {
    bool ok = *(it + 0) == 0x7f &&
              *(it + 1) == 'E' &&
              *(it + 2) == 'L' &&
              *(it + 3) == 'F';
    return ok ? ElfError::Ok : ElfHeaderInvalidMagicErr;
}

} // namespace efl
