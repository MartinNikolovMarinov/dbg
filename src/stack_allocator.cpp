#include <stack_allocator.h>

namespace {

struct StackFrame {
    StackFrame* prev;
};

} // namespace

void StackAllocator::setBuffer(void* buffer, addr_size cap) {
    Assert(buffer);
    Assert(cap > 0);

    m_startAddr = buffer;
    m_offset = 0;
    m_cap = cap;

    // Allocate space for the current frame marker
    StackFrame* currentFrame = reinterpret_cast<StackFrame*>(alloc(1, sizeof(StackFrame)));
    currentFrame->prev = nullptr;
}

void StackAllocator::resetBuffer() {
    Assert(m_startAddr);

    m_offset = 0;
    m_cap = 0;
    m_allocated = 0;
}

void* StackAllocator::alloc(addr_size count, addr_size size) {
    Assert(m_startAddr);
    Assert(m_offset + count * size < m_cap);

    void* ret = core::ptrAdvance(m_startAddr, m_offset);
    m_offset += count * size;
    m_allocated += count * size;
    return ret;
}

void* StackAllocator::calloc(addr_size count, addr_size size) {
    Assert(m_startAddr);

    void* ret = alloc(count, size);
    core::memset(reinterpret_cast<char*>(ret), char(0), count * size);
    return ret;
}

void StackAllocator::free(void*, addr_size, addr_size) {}

void StackAllocator::clear() {
    Assert(m_startAddr);

    resetBuffer();
    m_startAddr = nullptr;
}

addr_size StackAllocator::totalMemoryAllocated() {
    Assert(m_startAddr);
    return m_allocated;
}

addr_size StackAllocator::inUseMemory() {
    Assert(m_startAddr);
    return m_offset;
}

void StackAllocator::beginStackFrame() {
    Assert(m_startAddr);

    StackFrame* currentFrame = reinterpret_cast<StackFrame*>(m_startAddr);
    StackFrame* prevFrame = currentFrame;
    StackFrame* frame = reinterpret_cast<StackFrame*>(alloc(1, sizeof(StackFrame)));
    currentFrame = frame;
    currentFrame->prev = prevFrame;
}

void StackAllocator::freeStackFrame() {
    StackFrame* currentFrame = reinterpret_cast<StackFrame*>(m_startAddr);
    m_offset = addr_size(reinterpret_cast<char*>(currentFrame) -
                         reinterpret_cast<char*>(m_startAddr));
    currentFrame = currentFrame->prev;
}
