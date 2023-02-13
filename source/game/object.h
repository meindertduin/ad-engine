#pragma once

#include <cstdint>
#include <bitset>

#include "platform/gcc.h"
#include "constants.h"
#include "engine/format.h"

namespace game {
    class Ecs;
    using Signature = std::bitset<MaxComponents>;

    template<typename T>
    class Entity {
    public:
        Entity(uint32_t id, T *manager)
            : mId(id)
            , mManager(manager)
        {}

        [[nodiscard]] constexpr ALWAYS_INLINE uint32_t id() const {
            return mId;
        }

        bool operator==(const Entity &other) const {
            return mId == other.mId;
        }

        bool operator!=(const Entity &other) const {
            return mId != other.mId;
        }

        auto operator <=>(const Entity &other) const {
            return mId <=> other.mId;
        }

        template<typename C>
        void addComponent(C component) {
            mManager->template addComponent(*this, component);
        }

        template<typename C>
        C& getComponent() const {
            mManager->template getComponent<C>(*this);
        }
    private:
        uint32_t  mId;
        T *mManager;
    };

    using Object = Entity<Ecs>;
}


template<>
struct FormatType<game::Object> {
    static std::string format(const game::Object &value) {
        return formatString("Object({})", value.id());
    }
};

template<>
struct std::hash<game::Object> {
    auto operator()(const game::Object object) const -> size_t {
        return hash<uint32_t>()(object.id());
    }
};
