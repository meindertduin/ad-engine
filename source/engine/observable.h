#pragma once

#include <cstdint>
#include <unordered_map>
#include <functional>
#include <memory>
#include <map>
#include <utility>
#include "platform/gcc.h"
#include "logging.h"

template<typename T>
class Observer {
public:
    using Callback = std::function<void(const T &)>;
    using Unsubscribe = std::function<void(uint32_t)>;

    explicit Observer(uint32_t id, Callback &&callback, Unsubscribe unsubscribe)
        : mId(id)
        , mCallback(std::move(callback)),
        mUnsubscribe(std::move(unsubscribe))
    {
    }

    Observer(const Observer &) = delete;
    Observer& operator=(const Observer &) = delete;

    ~Observer() {
        mUnsubscribe(mId);
    }

    [[nodiscard]] constexpr ALWAYS_INLINE uint32_t id() const {
        return mId;
    }

    void notify(const T &subject) {
        mCallback(subject);
    }

    void unsubscribe() const {
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

        std::function<void(uint32_t)> unsubscribe = [this](uint32_t observerId) {
            if (this != nullptr) {
                mObservers.erase(observerId);
            }
        };

        auto observer = std::make_shared<Observer<T>>(id, std::move(callback), unsubscribe);

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
