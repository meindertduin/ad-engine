#pragma once

#include <cstdint>
#include <unordered_map>
#include <functional>
#include <memory>
#include <map>
#include "platform/gcc.h"


template<typename T>
class Observer {
public:
    using Callback = std::function<void(const T &)>;
    using Unsubscribe = std::function<void(uint32_t)>;

    Observer(uint32_t id, Callback callback, const Unsubscribe &unsubscribe)
        : mId(id)
        , mCallback(std::forward<std::function<void(const T &)>>(callback)),
        mUnsubscribe(unsubscribe)
    {
    }

    ~Observer() {
        mUnsubscribe(mId);
    }

    [[nodiscard]] constexpr ALWAYS_INLINE uint32_t id() const {
        return mId;
    }

    void notify(const T &subject) {
        mCallback(subject);
    }

    void unsubscribe() {
        mUnsubscribe(mId);
    }

private:
    uint32_t mId;
    std::function<void(const T &)> mCallback;
    std::function<void(uint32_t)> mUnsubscribe;
};

template<typename T>
class Observable {
public:
    using Callback = std::function<void(const T &)>;

    virtual ~Observable() = default;

    std::shared_ptr<Observer<T>> subscribe(Callback &&callback) {
        uint32_t id = mNextObserverId++;

        static std::function<void(uint32_t)> unsubscribe = [this](uint32_t id) {
            mObservers.erase(id);
        };

        auto observer = std::make_shared<Observer<T>>(id, std::forward<Callback>(callback), unsubscribe);

        mObservers.insert({ id, observer.get() });

        return observer;
    }

    void notify(const T &subject) {
        for (auto &observer : mObservers) {
            observer.second->notify(subject);
        }
    }
private:
    std::unordered_map<uint32_t , Observer<T>*> mObservers;
    uint32_t mNextObserverId { 0 };
};
