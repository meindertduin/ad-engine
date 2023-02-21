#pragma once

#include <cstdint>
#include "allocator.h"
#include "xxhash32.h"

template<typename K, typename T>
class HashMap {
private:
    struct KeyEntry {
        alignas (K) char key[sizeof(K)];
        bool valid;
    };

public:
    explicit HashMap(Allocator &allocator)
        : mAllocator(allocator)
    {
        initialize();
    }

    ~HashMap() {
        mAllocator.deallocate(mKeys);
        mAllocator.deallocate(mValues);
    }

    HashMap(const HashMap &rhs) = delete;
    HashMap& operator=(const HashMap &rhs) = delete;

    HashMap(HashMap &&rhs) noexcept
        : mAllocator(rhs.mAllocator)
    {
        mKeys = rhs.mKeys;
        mValues = rhs.mValues;
        mCapacity = rhs.mCapacity;
        mSize = rhs.mSize;
        mMask = rhs.mMask;

        rhs.mKeys = nullptr;
        rhs.mValues = nullptr;
        rhs.mCapacity = 0;
        rhs.mSize = 0;
        rhs.mMask = 0;
    }

    T& operator[](const K &key) {
        auto index = findIndex(key);
        return mValues[index];
    }

    void insert(const K &key, const T &value) {
        if (mSize >= mCapacity) {
            grow(mCapacity * 2);
        }

        const uint32_t hash = hashKey(key);
        auto index = hash % mMask;

        while (mKeys[index].valid) {
            index++;
        }

        if (index >= mCapacity) {
            index = 0;
            while (mKeys[index].valid) {
                index++;
            }
        }

        new (&mKeys[index].key) K(key);
        mValues[index] = value;

        mKeys[index].valid = true;

        mSize++;
    }
private:
    Allocator &mAllocator;
    uint32_t mSize;
    uint32_t mCapacity;
    uint32_t mMask;

    KeyEntry *mKeys;
    T *mValues;

    void initialize() {
        mSize = 0;
        mCapacity = 16;
        mMask = mCapacity - 1;

        mKeys = static_cast<KeyEntry*>(mAllocator.allocate(sizeof(KeyEntry) * (mCapacity + 1), alignof(KeyEntry)));
        mValues = static_cast<T*>(mAllocator.allocate(sizeof(T) * mCapacity, alignof(T)));

        for (uint32_t i = 0; i < mCapacity; i++) {
            mKeys[i].valid = true;
        }

        mKeys[mCapacity].valid = false;
    }

    void grow(uint32_t newCapacity) {

    }

    uint32_t hashKey(const K &key) {
        return XXHash32::hash(&key, sizeof(key), 283991);
    }

    uint32_t findIndex(const K &key) {
        const uint32_t hash = hashKey(key);
        auto index = hash % mMask;

        while (mKeys[index].valid) {
            if (key == *reinterpret_cast<K *>(&mKeys[index].key)) {
                return index;
            }
            index++;
        }

        if (index != mCapacity) {
            return mCapacity;
        }

        index = 0;
        while (mKeys[index].valid) {
            if (key == *reinterpret_cast<K *>(&mKeys[index].key)) {
                return index;
            }
            index++;
        }

        return mCapacity;
    }
};