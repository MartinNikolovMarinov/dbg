#pragma once

#include <basic.h>

struct StackAllocator {
    NO_COPY(StackAllocator);

    StackAllocator() = default;

    void setBuffer(void* buffer, addr_size cap);
    void resetBuffer();

    void* alloc(addr_size count, addr_size size);
    void* calloc(addr_size count, addr_size size);
    void free(void* ptr, addr_size count, addr_size size); // does nothing
    void clear();
    addr_size totalMemoryAllocated();
    addr_size inUseMemory();

    void beginStackFrame();
    void freeStackFrame();

    void* m_startAddr = nullptr;
    addr_size m_offset = 0;
    addr_size m_cap = 0;
    addr_size m_allocated = 0;
};
static_assert(core::AllocatorConcept<StackAllocator>);

