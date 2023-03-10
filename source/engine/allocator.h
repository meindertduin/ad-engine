#pragma once

#include <cstddef>
#include <memory>
#include <list>
#include "singly_linked_list.h"

constexpr std::size_t DefaultAlignment = 8;

class Allocator {
public:
    explicit Allocator(const std::size_t totalSize)
        : mTotalSize(totalSize)
    {}

    virtual void* allocate(std::size_t size, std::size_t alignment) = 0;
    virtual void deallocate(void* ptr) = 0;
protected:
    bool mFreeOnDestruction { false };
    std::size_t mTotalSize { 0 };
    void *mStart { nullptr };
    std::size_t mUsed { 0 };
    std::size_t mPeak { 0 };
};

class ListAllocator : public Allocator {
public:
    explicit ListAllocator(std::size_t size);
    ~ListAllocator();

    void* allocate(std::size_t size, std::size_t alignment) override;
    void deallocate(void* pointer) override;
    void reset();
private:
    struct FreeBlock {
        std::size_t size { 0 };
    };

    struct AllocatedBlock {
        std::size_t size;
        uint8_t padding;
    };

    using Node = SinglyLinkedList<FreeBlock>::Node;
    SinglyLinkedList<FreeBlock> mFreeBlocks = SinglyLinkedList<FreeBlock>();

    void tryMergeFreedBlock(Node* freeNode, Node* previousNode);
};