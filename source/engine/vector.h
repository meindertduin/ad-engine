#pragma once

#include "allocator.h"
#include "platform/gcc.h"

template<typename T>
class Vector {
public:
   explicit Vector(Allocator &allocator, const std::size_t initialCapacity = 0)
      : mAllocator(allocator)
      , mCapacity(initialCapacity)
   {
      if (mCapacity > 0) {
         mData = static_cast<T*>(mAllocator.allocate(mCapacity * sizeof(T), alignof(T)));
      }
   }

   Vector(Vector &&rhs) noexcept
      : mAllocator(rhs.mAllocator)
      , mCapacity(0)
   {
       auto i = rhs.mCapacity;
       rhs.mCapacity = mCapacity;
       mCapacity = i;

       i = mSize;
       mSize = rhs.mSize;
       rhs.mSize = i;

       T* p = rhs.mData;
       rhs.mData = mData;
       mData = p;
   }

   ~Vector() {
       if (!mData) {
           return;
       }

       destructItems();
       mAllocator.deallocate(mData);
   }

   Vector(const Vector &rhs) = delete;
   Vector& operator=(const Vector &rhs) = delete;

   constexpr ALWAYS_INLINE T* begin() const { return mData; }
   constexpr ALWAYS_INLINE T* end() const {
       if (mData == nullptr) {
           return nullptr;
       }

       return mData + mSize;
   }

   constexpr ALWAYS_INLINE T* data() {
       return mData;
   }

   constexpr ALWAYS_INLINE T& operator[](const std::size_t index) const {
       return mData[index];
   }

   [[nodiscard]] constexpr ALWAYS_INLINE std::size_t size() const { return mSize; }

   void push(T&& value) {
       auto size = mSize;
       if (size == mCapacity) {
           expand();
       }

       new (mData + size) T(std::move(value));

       size++;
       mSize = size;
   }

   void push(const T& value) {
       auto size = mSize;
       if (size == mCapacity) {
           expand();
       }

       new (mData + size) T(value);

       size++;
       mSize = size;
   }

   void reserve(std::size_t capacity) {
        if (capacity > mCapacity) {
            auto newCapacity = capacity;
            auto newSize = mSize;
            auto newData = static_cast<T*>(mAllocator.allocate(newCapacity * sizeof(T), alignof(T)));
            if (mData != nullptr) {
                for (auto i = 0; i < mSize; ++i) {
                    new (newData + i) T(std::move(mData[i]));
                }

                mAllocator.deallocate(mData);
            }
            mData = newData;
            mCapacity = newCapacity;
            mSize = newSize;
        }
   }

   void clear() {
       destructItems();
       mSize = 0;
   }

private:
    Allocator &mAllocator;
    T *mData { nullptr };
    std::size_t mCapacity;
    std::size_t mSize { 0 };

    void destructItems(int from = 0, int to = -1) {
        if (to == -1) {
            to = mSize;
        }

        for (auto i = from; i < to; i++) {
            mData[i].~T();
        }
    }

    void expand() {
        reserve(mCapacity < 4 ? 4 : mCapacity * 1.5);
    }
};