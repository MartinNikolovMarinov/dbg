#include <dbg.h>

struct DbgErrorDetails {};

enum struct DbgErrorCode : i32 {
    OK = 0,
};

struct DbgError {
    core::UniquePtr<DbgErrorDetails> err;
    DbgErrorCode code = DbgErrorCode::OK;
};

struct DbgInputBuffer {
    core::ArrList<u8> bytes;
    addr_size ridx;

    static DbgInputBuffer createFromFile(const char*) {
        Panic(false, "Not implemented yet");
        return {};
    }
};

void readELFSections(const char*) {
    // Elf64_Ehdr header;
}

struct Example {
    i32 a;
    bool b;
    char c;
};

i32 main() {
    core::initProgramCtx(assertHandler, nullptr);

    // auto ibuff = InputBuffer::createFromFile(DBG_TEST_BINARIES_DIR"/data/test_binaries/simplest_64bit_program.o");/

    constexpr addr_size cap = 1024;
    char buff[cap];
    core::memset(buff, char(9), cap);
    StackAllocator s;
    s.setBuffer(buff, cap);

    {
        char* data = reinterpret_cast<char*>(s.alloc(3, sizeof(char)));
        data[0] = 'o';
        data[1] = 'u';
        data[2] = 't';
        data[3] = '\0';
    }

    {
        s.beginStackFrame();
        defer { s.freeStackFrame(); };
        char* data = reinterpret_cast<char*>(s.alloc(2, sizeof(char)));
        data[0] = 'i';
        data[1] = 'n';
        data[2] = '\0';
    }

    {
        char* data = reinterpret_cast<char*>(s.alloc(5, sizeof(char)));
        data[0] = 'a';
        data[1] = 'f';
        data[2] = 't';
        data[3] = 'e';
        data[4] = 'r';
        data[5] = '\0';
    }

    s.clear();

    return 0;
}
