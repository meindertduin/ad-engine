#pragma once

#include <cstddef>
#include <memory>
#include <list>
#include "singly_linked_list.h"

constexpr std::size_t DefaultAlignment = 8;

class Allocator {
public:
    virtual void* allocate(std::size_t size) = 0;
    virtual void deallocate(void* ptr) = 0;
};

class ListAllocator : public Allocator {
public:
    enum class PlacementPolicy {
        First,
        Best,
    };

    ListAllocator(std::size_t size, PlacementPolicy policy);
    ~ListAllocator();

    void* allocate(std::size_t size) override;
    void deallocate(void* ptr) override;
    void reset();
private:
    struct FreeBlock {
        std::size_t size;
        FreeBlock* next;
    };

    struct AllocatedBlock {
        std::size_t size;
        uint8_t padding;
    };

    using Node = SinglyLinkedList<FreeBlock>::Node;
    SinglyLinkedList<FreeBlock> mFreeBlocks;

    using FreeHeadersList = std::list<FreeBlock*>;
    FreeHeadersList mFreeHeaders;

    void findFreeHeader(std::size_t size, std::size_t alignment, std::size_t &padding, Node* &foundNode, Node* &previous) const;
    void coalescene(Node* free_node, Node* previous_node);
};