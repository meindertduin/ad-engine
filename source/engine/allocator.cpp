#include "allocator.h"

inline constexpr std::size_t calculatePadding(std::size_t size, std::size_t alignment) {
    auto remainder = size % alignment;
    if (remainder > 0) {
        return alignment - remainder;
    }

    return 0;
}

inline constexpr std::size_t calculatePadding(std::size_t size, std::size_t alignment, std::size_t headerSize) {
    if (alignment == 0)
        return headerSize;

    auto padding = calculatePadding(size, alignment);
    auto needed_space = headerSize;

    if (padding < needed_space) {
        needed_space -= padding;

        // How many alignments I need to fit the header
        if(needed_space % alignment > 0){
            padding += alignment * (1+(needed_space / alignment));
        }else {
            padding += alignment * (needed_space / alignment);
        }
    }

    return padding;
}

ListAllocator::ListAllocator(std::size_t size)
    : Allocator(size)
{
    mStart = malloc(size);
    reset();
}

ListAllocator::~ListAllocator()
{
    free(mStart);
}

void *ListAllocator::allocate(std::size_t size, std::size_t alignment) {
    auto padding = calculatePadding(mUsed, alignment, sizeof(AllocatedBlock));
    auto requiredSize = size + padding + sizeof(AllocatedBlock);

    auto* current = mFreeBlocks.head;
    Node* previous = nullptr;

    while (current != nullptr) {
        if (current->data.size >= requiredSize) {
            // Found a block that fits
            break;
        }

        // Move to next block
        previous = current;
        current = current->next;
    }

    if (current->data.size < requiredSize) {
        // No block with enough memory found
        throw std::bad_alloc();
    }

    // Calculate the new free block size
    auto newFreeBlockSize = current->data.size - requiredSize;
    if (newFreeBlockSize > 0) {
        // TODO - what if the new free block is smaller than the header size?
        // There is space left for a new free block
        auto currentAddress = (std::size_t) current;
        auto newFreeBlock = (Node*)(currentAddress + requiredSize);

        newFreeBlock->data.size = newFreeBlockSize;
        newFreeBlock->next = current->next;

        // Insert the new free block
        mFreeBlocks.insert(newFreeBlock, current);
    }

    // Remove the node from the free-headers list
    mFreeBlocks.remove(current, previous);

    // Calculate the address of the new allocated block
    auto headerAddress = (std::size_t)(current);

    ((AllocatedBlock*) headerAddress)->size = size;
    ((AllocatedBlock*) headerAddress)->padding = padding;

    auto dataAddress = headerAddress + sizeof(AllocatedBlock);

    mUsed += requiredSize;
    mPeak = std::max(mPeak, mUsed);

    return (void*) dataAddress;
}

void ListAllocator::deallocate(void *pointer) {
    auto currentAddress = (std::size_t)(pointer);
    auto headerAddress = currentAddress - sizeof(AllocatedBlock);
    auto allocatedHeader = (AllocatedBlock*)(headerAddress);

    auto blockSize = allocatedHeader->size + allocatedHeader->padding + sizeof(AllocatedBlock);

    auto newFreeBlock = (Node*)(headerAddress);
    newFreeBlock->data.size = blockSize;
    newFreeBlock->next = nullptr;

    auto* current = mFreeBlocks.head;
    Node* previous = nullptr;

    while (current != nullptr) {
        if ((std::size_t)(current) >= headerAddress) {
            // Found the block that is after the new free block
            mFreeBlocks.insert(newFreeBlock, previous);
            break;
        }

        // Move to next block
        previous = current;
        current = current->next;
    }

    mUsed -= blockSize;

    tryMergeFreedBlock(newFreeBlock, previous);
}

void ListAllocator::reset() {
    mUsed = 0;
    mPeak = 0;
    auto firstNode = (Node*) mStart;
    firstNode->data.size = mTotalSize;
    firstNode->next = nullptr;
    mFreeBlocks.head = nullptr;
    mFreeBlocks.insert(firstNode, nullptr);
}

void ListAllocator::tryMergeFreedBlock(ListAllocator::Node *freeNode, ListAllocator::Node *previousNode) {
    auto nextNode = freeNode->next;

    if (nextNode != nullptr) {
        auto nextNodeAddress = (std::size_t)(nextNode);
        auto freeNodeAddress = (std::size_t)(freeNode);

        if (nextNodeAddress == freeNodeAddress + freeNode->data.size) {
            // Merge with the next block
            freeNode->data.size += nextNode->data.size;
            mFreeBlocks.remove(nextNode, freeNode);
        }
    }

    if (previousNode != nullptr) {
        auto previousNodeAddress = (std::size_t)(previousNode);
        auto freeNodeAddress = (std::size_t)(freeNode);

        if (previousNodeAddress + previousNode->data.size == freeNodeAddress) {
            // Merge with the previous block
            previousNode->data.size += freeNode->data.size;
            mFreeBlocks.remove(freeNode, previousNode);
        }
    }
}
