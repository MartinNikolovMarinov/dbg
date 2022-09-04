#include "elf_parser.h"

namespace elf
{

ElfParser::ElfParser(ByteBuff&& bytes) : m_bytes(bytes), m_headerIt(m_bytes.end()), m_is64Bit(false) {}

ElfError ElfParser::Parse() {
    if (m_bytes.size() < sizeof(ElfHeader32_Packed)) {
        return ElfHeaderInvalidSize;
    }

    BytesIt parseIt = m_bytes.begin();
    m_headerIt = parseIt;
    if (auto err = CheckMagic(parseIt); err.IsErr()) {
        return err;
    }
    if (auto err = ParseElfHeader(parseIt); err.IsErr()) {
        return err.Err();
    }

    return ElfError::Ok;
}

const ElfHeader32_Packed& ElfParser::GetElf32Header() const {
    Assert(m_headerIt != m_bytes.end(), "Elf header not parsed");
    Assert(m_is64Bit, "Elf header is NOT 32 bit");
    ElfHeader32_Packed* ret = reinterpret_cast<ElfHeader32_Packed*>(m_headerIt.base());
    return *ret;
}

const ElfHeader64_Packed& ElfParser::GetElf64Header() const {
    Assert(m_headerIt != m_bytes.end(), "Elf header not parsed");
    Assert(m_is64Bit, "Elf header is NOT 64 bit");
    ElfHeader64_Packed* ret = reinterpret_cast<ElfHeader64_Packed*>(m_headerIt.base());
    return *ret;
}

ElfError ElfParser::ParseElfHeader(BytesIt& it) {
    if (*it == ElfClass::CLASSNONE) {
        return ElfHeaderInvalidClassErr;
    }
    m_is64Bit = (*it == ElfClass::ELF64);
    if (m_is64Bit) {
        auto ret = GetElf64Header();
        if (ret.ehsize != sizeof(ElfHeader64_Packed)) {
            // ElfClass lied to me !
            return ElfHeaderIsNot64BitErr;
        }
    } else {
        auto ret = GetElf32Header();
        if (ret.ehsize != sizeof(ElfHeader32_Packed)) {
            // ElfClass lied to me !
            return ElfHeaderIsNot32BitErr;
        }
    }
    return ElfError::Ok;
}

ElfError ElfParser::CheckMagic(BytesIt& it) {
    bool ok = *(it + 0) == 0x7f &&
              *(it + 1) == 'E' &&
              *(it + 2) == 'L' &&
              *(it + 3) == 'F';
    it += 4;
    return ok ? ElfError::Ok : ElfHeaderInvalidMagicErr;
}

} // namespace efl
