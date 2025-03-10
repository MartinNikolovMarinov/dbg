#pragma once

#include <basic.h>

template <typename T>
struct MemoryPool {
    struct Node {
        Node* next;
        T* addr;
    };

    static constexpr Node USED = {};
    static constexpr addr_size dataBlockSize = core::align(sizeof(T), alignof(T));

    T* alloc() {
        Assert(head);

        if (head->next == nullptr) {
            Assert(false, "TODO: Resize?");
        }

        Node* curr = head;
        head = head->next;
        curr->next = &USED;
        len++;
        return curr->addr;
    }

    void free(T* addr) {
    }

    void clear() {
    }

    addr_size totalMemoryAllocated() {
        return cap * dataBlockSize + cap * sizeof(Node);
    }

    addr_size inUseMemory() {
        return len * dataBlockSize + cap * sizeof(Node);
    }

    Node* head;
    Node* nodes;
    T* data;
    addr_size len;
    addr_size cap;
};

template <typename T>
PoolAllocator<T> makePool(addr_size initialCap) {
    Assert(initialCap > 5); // avoiding edge cases for 1-2 element linked lists.

    // TODO: use the core allocator
    PoolAllocator<T> pa;
    pa.data = std::malloc(initialCap * sizeof(PoolAllocator<T>::dataBlockSize));
    pa.nodes = std::malloc(initialCap * sizeof(PoolAllocator<T>::Header));
    pa.cap = initialCap;
    pa.len = 0;

    for (addr_size i = 0; i < pa.cap - 1; i++) {
        auto& node = pa.nodes[i];
        node.next = pa.nodes[i+1];
        node.addr = pa.data + i;
    }

    pa.nodes[pa.cap - 1].next = nullptr;
    pa.head = pa.nodes[0];

    return pa;
}
