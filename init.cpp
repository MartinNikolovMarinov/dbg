#include "init.h"

#include <core.h>

#include <iostream>

namespace dbg
{

using namespace coretypes;

namespace
{

static constexpr i32 MAX_ON_ASSERT_FNS = 10;
static i32 currentAssertFn = 0;
static void(*onAssertFns[MAX_ON_ASSERT_FNS])();

bool GlobalAssertHandler(const char* failedExpr,
                         const char* file, i32 line,
                         const char* errMsg) {
    std::cout << "[ASSERTION] [EXPR]: " << failedExpr
              << " [FILE]: " << file
              << " [LINE]: " << line
              << " [MSG]: " << errMsg
              << std::endl; // flush stream!
    for (i32 i = 0; i < currentAssertFn; i++) {
        onAssertFns[i]();
    }
    return true;
}

} // namespace

void OnAssertCb(void(*fn)()) {
    onAssertFns[currentAssertFn++] = fn;
}

void Init() {
    core::SetGlobalAssertHandler(&GlobalAssertHandler);
}

} // namespace dbg
