#pragma once

#include <cstdint>
#include "allocator.h"
#include "xxh3/xxhash32.h"

template<typename K, typename T>
class HashMap {
private:
    struct KeyEntry {
        alignas (K) char key[sizeof(K)];
        bool valid;
    };

public:
    class Iterator {
    public:
        Iterator(HashMap<K, T> &map, std::size_t index)
            : mMap(map)
            , mIndex(index)
        {
        }

        Iterator(const Iterator &rhs) = default;
        Iterator& operator=(const Iterator &rhs) = default;

        Iterator& operator++() {
            mIndex++;
            while (mIndex < mMap.mCapacity && !mMap.mKeys[mIndex].valid) {
                mIndex++;
            }

            return *this;
        }

        Iterator operator++(int) {
            Iterator tmp(*this);
            operator++();
            return tmp;
        }

        bool operator==(const Iterator &rhs) const {
            return mIndex == rhs.mIndex;
        }

        T& operator*() {
            return mMap.mValues[mIndex];
        }

        T* operator->() {
            return &mMap.mValues[mIndex];
        }

        K& key() {
            return *reinterpret_cast<K*>(mMap.mKeys[mIndex].key);
        }

        T& value() {
            return mMap.mValues[mIndex];
        }
    private:
        HashMap<K, T> &mMap;
        std::size_t mIndex;
    };

    explicit HashMap(Allocator &allocator, uint32_t initialCapacity = 16)
        : mAllocator(allocator)
    {
        initialize(initialCapacity);
    }

    ~HashMap() {
        if (!mKeys) {
            return;
        }
        
        for (auto i = 0; i < mCapacity; i++) {
            if (mKeys[i].valid) {
                reinterpret_cast<K*>(mKeys[i].key)->~K();
                auto &value = mValues[i];
                value.~T();
                mKeys[i].valid = false;
            }
        }

        mAllocator.deallocate(mKeys);
        mAllocator.deallocate(mValues);
    }

    HashMap(const HashMap &rhs) = delete;
    HashMap& operator=(const HashMap &rhs) = delete;

    HashMap(HashMap &&rhs) noexcept
        : mAllocator(rhs.mAllocator)
        , mKeys(rhs.mKeys)
        , mValues(rhs.mValues)
        , mCapacity(rhs.mCapacity)
        , mSize(rhs.mSize)
        , mMask(rhs.mMask)
    {
        rhs.mKeys = nullptr;
        rhs.mValues = nullptr;
        rhs.mCapacity = 0;
        rhs.mSize = 0;
        rhs.mMask = 0;
    }

    HashMap& operator=(HashMap &&rhs) noexcept {
        mAllocator = rhs.mAllocator;
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

        return *this;
    }

    T& operator[](const K &key) {
        auto index = findIndex(key);
        return mValues[index];
    }

    Iterator begin() {
        for (auto i = 0; i < mCapacity; ++i) {
            if (mKeys[i].valid) {
                return Iterator(*this, i);
            }
        }

        return { *this, mCapacity };
    }

    Iterator end() {
        return { *this, mCapacity };
    }

    void insert(const K &key, const T &value) {
        if (mSize >= mCapacity * 0.75) {
            grow(mCapacity * 2);
        }

        const uint32_t hash = hashKey(key);
        auto index = hash % mMask;

        while (mKeys[index].valid) {
            index++;
        }

        if (index == mCapacity) {
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

    void remove(const K &key) {
        auto index = findIndex(key);
        if (index == mCapacity) {
            return;
        }

        mKeys[index].valid = false;
        mValues[index].~T();
        mSize--;

        index = (index + 1) % mMask; // wrap around
        while (mKeys[index].valid) {
            rehash(index);
            index = (index + 1) % mCapacity;
        }
    }

    Iterator find(const K &key) {
        auto index = findIndex(key);
        if (index == mCapacity) {
            return end();
        }

        return Iterator(*this, index);
    }

    bool contains(const K &key) const {
        auto index = findIndex(key);
        return index != mCapacity;
    }

private:
    Allocator &mAllocator;
    uint32_t mSize { 0 };
    uint32_t mCapacity { 0 };
    uint32_t mMask { 0 };

    KeyEntry *mKeys { nullptr };
    T *mValues { nullptr };

    void initialize(uint32_t capacity) {
        mSize = 0;
        mCapacity = capacity;
        mMask = mCapacity - 1;

        mKeys = static_cast<KeyEntry*>(mAllocator.allocate(sizeof(KeyEntry) * (mCapacity + 1), alignof(KeyEntry)));
        mValues = static_cast<T*>(mAllocator.allocate(sizeof(T) * mCapacity, alignof(T)));

        for (uint32_t i = 0; i <= mCapacity; i++) {
            mKeys[i].valid = false;
        }
    }

    void grow(uint32_t newCapacity) {
        HashMap<K, T> newMap(mAllocator, newCapacity);

        for (uint32_t i = 0; i < mCapacity; i++) {
            if (mKeys[i].valid) {
                newMap.insert(*reinterpret_cast<K *>(&mKeys[i].key), mValues[i]);
            }
        }

        *this = std::move(newMap);
    }

    uint32_t findEmptyKeyValue(const K &key, uint32_t endIndex) const {
        const uint32_t hash = hashKey(key);
        auto index = hash % mMask;

        while (mKeys[index].valid && index != endIndex) {
            index++;
        }

        if (index >= mCapacity) {
            index = 0;
            while (mKeys[index].valid && index != endIndex) {
                index++;
            }
        }

        return index;
    }

    void rehash(uint32_t index) {
        K& key = *(K*) &mKeys[index].key;
        auto rehashedIndex = findEmptyKeyValue(key, index);

        if (rehashedIndex != index) {
            new (&mKeys[rehashedIndex].key) K(key);
            mValues[rehashedIndex] = mValues[index];

            mKeys[index].valid = false;
            mValues[index].~T();
            mKeys[rehashedIndex].valid = true;
        }
    }

    uint32_t hashKey(const K &key) const {
        return XXHash32::hash(&key, sizeof(key), 283991);
    }

    uint32_t findIndex(const K &key) const {
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