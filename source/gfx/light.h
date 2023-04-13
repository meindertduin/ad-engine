#pragma once

#include <glm/glm.hpp>
#include "gpu/gpu.h"
#include "shader.h"

namespace gfx {
    struct DirLight {
        glm::vec3 direction;
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
    };

    struct PointLight {
        glm::vec3 position;
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
        float constant;
        float linear;
        float quadratic;
    };

    constexpr int MaxDirLights = 4;
    constexpr int MaxPointLights = 8;

    class Lights {
    public:
        Lights() = default;
        Lights(const std::vector<DirLight> &dirLights);

        void setBufferData();
    private:
        std::unique_ptr<gpu::SharedUniformBuffer> mBuffer;
        std::unique_ptr<gpu::BufferLayout> mLayout;

        int mDirLightsCount;
        std::vector<DirLight> mDirLights;
    };

}
