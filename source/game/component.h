#pragma once

#include <cstdint>
#include <stdexcept>

namespace game {
    using ComponentType = uint32_t;

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

    namespace detail {
        inline uint32_t sComponentCount = 0;

        template<typename T>
        struct ComponentTypeRegister {
            ComponentTypeRegister() {
                registerType();
            }

            ComponentType registerType() {
                if (sComponentCount >= 32) {
                    throw std::runtime_error("Too many components");
                }

                T::mType = sComponentCount++;
                printf("Registered component type with id %d\n", T::mType);
            }
        };
    }

#define REGISTER_COMPONENT(type) \
    static inline game::detail::ComponentTypeRegister<type> gComponentTypeRegister_##type;
}