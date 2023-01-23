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

ListAllocator::ListAllocator(std::size_t size, ListAllocator::PlacementPolicy policy)
    : Allocator(size)
    , mPolicy(policy)
{
    mStart = malloc(size);
    reset();
}

ListAllocator::~ListAllocator()
{
    free(mStart);
}

void *ListAllocator::allocate(std::size_t size, std::size_t alignment) {
    std::size_t padding;
    Node* foundNode, *previous;
    findFreeHeader(size, alignment, padding, foundNode, previous);

    if (foundNode == nullptr) {
        throw std::bad_alloc();
    }

    std::size_t alignmentPadding = padding - sizeof(AllocatedBlock);
    std::size_t requiredSize = size + padding;

    std::size_t rest = foundNode->data.size - requiredSize;

    if (rest > 0) {
        auto newFreeNode = (Node*)((std::size_t) foundNode + requiredSize);
        newFreeNode->data.size = rest;
        mFreeBlocks.insert(newFreeNode, foundNode);
    }

    mFreeBlocks.remove(foundNode, previous);

    // set up the data block
    std::size_t headerAddress = (std::size_t) foundNode + alignmentPadding;
    std::size_t dataAddress = headerAddress + sizeof(AllocatedBlock);

    ((AllocatedBlock*) headerAddress)->size = requiredSize;
    ((AllocatedBlock*) headerAddress)->padding = static_cast<uint8_t>(padding);

    mUsed += requiredSize;
    mPeak = std::max(mPeak, mUsed);

    return (void*) dataAddress;
}

void ListAllocator::deallocate(void *pointer) {
    std::list<FreeBlock> list;

    auto currentAddress = (std::size_t) pointer;
    auto headerAddress = currentAddress - sizeof(AllocatedBlock);
    AllocatedBlock* allocationHeader {(AllocatedBlock*) headerAddress };

    Node* freeNode = (Node* ) headerAddress;
    freeNode->data.size = allocationHeader->size + allocationHeader->padding;
    freeNode->next = nullptr;

    auto it = mFreeBlocks.head;
    Node* it_prev = nullptr;

    while (it != nullptr) {
        if (pointer < it) {
            mFreeBlocks.insert(freeNode, it_prev);
            break;
        }

        it_prev = it;
        it = it->next;
    }

    mUsed -= freeNode->data.size;

    coalescene(freeNode, it_prev);
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

void ListAllocator::findFreeHeader(std::size_t size, std::size_t alignment, size_t &padding, ListAllocator::Node *&foundNode, ListAllocator::Node *&previous) const {
    padding = calculatePadding(size, alignment, sizeof(AllocatedBlock));

    auto it = mFreeBlocks.head;
    Node* it_previous = nullptr;

    while (it != nullptr) {
        if (it->data.size >= size + padding) {
            break;
        }
        it_previous = it;
        it = it->next;
    }

    foundNode = it;
    previous = it_previous;
}

void ListAllocator::coalescene(ListAllocator::Node *freeNode, ListAllocator::Node *previousNode) {
    if (freeNode->next != nullptr &&
        (std::size_t) freeNode + freeNode->data.size == (std::size_t) freeNode->next)
    {
        freeNode->data.size += freeNode->next->data.size;
        mFreeBlocks.remove(freeNode->next, freeNode);
    }

    if (previousNode != nullptr &&
        (std::size_t) previousNode + previousNode->data.size == (std::size_t) freeNode)
    {
        previousNode->data.size += freeNode->data.size;
        mFreeBlocks.remove(freeNode, previousNode);
    }
}
