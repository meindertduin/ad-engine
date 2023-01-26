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

    auto* current = reinterpret_cast<FreeBlock*>(mFreeBlocks.head);
    FreeBlock* previous = nullptr;

    while (current != nullptr) {
        if (current->size >= requiredSize) {
            // Found a block that fits

            // Calculate the new free block size
            auto newFreeBlockSize = current->size - requiredSize;
            if (newFreeBlockSize > 0) {
                // There is space left for a new free block
                auto newFreeBlock = (FreeBlock*)(current + requiredSize);
                newFreeBlock->size = newFreeBlockSize;
                newFreeBlock->next = current->next;

                // Insert the new free block
                mFreeBlocks.insert(newFreeBlock, previous);
            }

            // Remove the node from the free-headers list
            mFreeBlocks.remove(current, previous);
        }

        // Move to next block
        previous = current;
        current = reinterpret_cast<FreeBlock*>(current->next);
    }

    return nullptr;
}

void ListAllocator::deallocate(void *pointer) {

}

void ListAllocator::reset() {
    mUsed = 0;
    mPeak = 0;
    auto firstNode = (FreeBlock*) mStart;
    firstNode->size = mTotalSize;
    firstNode->next = nullptr;
    mFreeBlocks.head = nullptr;
    mFreeBlocks.insert(firstNode, nullptr);
}