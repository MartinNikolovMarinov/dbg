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

void readELFSections(const char* filename) {
    Elf64_Ehdr header;
}

struct Example {
    i32 a;
    bool b;
    char c;
};

i32 main() {
    core::initProgramCtx(assertHandler, nullptr);

    // auto ibuff = InputBuffer::createFromFile(DBG_TEST_BINARIES_DIR"/data/test_binaries/simplest_64bit_program.o");/

    auto& poolAllocator = createPoolAllocator<Example>(100, 10000);

    return 0;
}
