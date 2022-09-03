#include <core.h>
#include <core_error.h>
#include <io.h>
#include <plt.h>

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

// TODO: Should I continue working with FAT Errors ?

struct File {
    static constexpr u32 DEFAULT_BUF_SIZE = 1024;

    static constexpr std::string_view ERR_EOF = "EOF";

    File() = default;
    File(core::plt::FileDesc&& file) : m_file(std::move(file)) {}

    // Prevent copy
    File(const File& other) = delete;
    File& operator=(const File& other) = delete;

    // Allow move
    File(File&& other) : m_file(std::move(other.m_file)), m_buf(std::move(other.m_buf)) {}

    // Read
    struct ReadResponse {
        ReadResponse() : m_n(0), m_err(core::error::Error()) {}

        bool IsErr() const { return m_err.IsErr(); }
        core::error::Error Err() const { return m_err; }
        i64 N() const { return m_n; }

    private:
        friend struct File;
        i64 m_n;
        core::error::Error m_err;
    };

    [[nodiscard]] ReadResponse Read(void* out, u64 size) {
        ReadResponse ret;
        if (size == 0) return ret;
        if (m_buf.empty()) m_buf.resize(DEFAULT_BUF_SIZE);

        u64 bytesToRead = std::min(m_buf.size(), size);
        auto rr = core::plt::OsRead(m_file, m_buf.data(), bytesToRead);
        if (!rr.b.isOk()) {
            ret.m_err = core::error::Error(std::string("failed to read file reason: ") + strerror(errno));
            ret.m_n = 0;
            return ret;
        }
        i64 readBytes = rr.a;
        if (readBytes == 0) {
            ret.m_err = {ERR_EOF};
            ret.m_n = 0;
            return ret;
        }

        std::memcpy(out, m_buf.data(), readBytes);
        ret.m_err = {};
        ret.m_n = readBytes;
        return ret;
    }

    // Close
    struct CloseResponse {
        CloseResponse() : m_err(core::error::Error()) {}

        bool IsErr() const { return m_err.IsErr(); }
        core::error::Error Err() const { return m_err; }

    private:
        friend struct File;
        core::error::Error m_err;
    };

    [[nodiscard]] CloseResponse Close() {
        CloseResponse ret;
        if (!core::plt::OsClose(m_file).isOk()) {
            ret.m_err = core::error::Error(std::string("failed to close file reason: ") + strerror(errno));
            return ret;
        }
        return ret;
    };

private:
    core::plt::FileDesc m_file;
    std::vector<u8> m_buf;
};

core::error::ErrorValue<File> OpenFile(std::string_view path, u64 flag, u64 mode) {
    auto openRes =  core::plt::OsOpen(path.data(), flag, mode);
    if (!openRes.b.isOk()) {
        return { {}, { std::string("failed to open file: \"") + path.data() + std::string("\"; reason: ") + std::strerror(errno) }};
    }
    core::plt::FileDesc fd = openRes.a;
    File file(std::move(fd));
    return { std::move(fd), {} };
}

namespace core::io {

using Error = core::error::Error;

template<> bool  core::io::IsErr(const File::ReadResponse& res)  { return res.IsErr(); }
template<> Error core::io::Err(const File::ReadResponse& res)    { return res.Err(); }
template<> u64   core::io::N(const File::ReadResponse& res)      { return res.N(); }

template<> File::ReadResponse core::io::Read(File& r, void* buf, u64 size) { return r.Read(buf, size); }

template<> File::CloseResponse core::io::Close(File& c) { return c.Close(); }

} // namsepcae io

i32 main(i32, const char**, const char**) {
    auto res = OpenFile("../main.cpp", O_RDONLY, 0);
    if (res.IsErr()) {
        std::cout << res.err.msg << std::endl;
        return 1;
    }

    auto file = std::move(res.val);
    constexpr u64 SIZE = 180;

    u8 buf[SIZE];
    auto readRes = core::io::Read<File, File::ReadResponse>(file, buf, SIZE);
    if (core::io::IsErr(readRes)) {
        std::cout << core::io::Err(readRes).msg << std::endl;
        return 1;
    }
    buf[SIZE - 1] = '\0';

    std::cout << buf << std::endl;
    std::cout << core::io::N(readRes) << std::endl;

    if (auto res = core::io::Close<File, File::CloseResponse>(file); res.IsErr()) {
        std::cout << res.Err().msg << std::endl;
        return 1;
    }

    return 0;
}
