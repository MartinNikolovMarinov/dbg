#include <dbg.h>

i32 main() {
    core::initProgramCtx(assertHandler, nullptr);

    logInfo("{}", 5.3);

    return 0;
}
