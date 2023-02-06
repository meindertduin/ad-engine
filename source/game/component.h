#pragma once

#include <cstdint>
#include "constants.h"
#include "component_registry.h"

namespace game {
    namespace detail {
        template <typename T>
        class ComponentTypeRegister;
    }

    template<typename T>
    class Component {
    public:
        virtual ~Component() = default;

        [[nodiscard]] static ComponentType type() {
            return mType;
        };
    private:
        inline static ComponentType mType;

        friend class detail::ComponentTypeRegister<T>;
    };

#define REGISTER_COMPONENT(type) \
    static inline game::detail::ComponentTypeRegister<type> gComponentTypeRegister_##type;
}