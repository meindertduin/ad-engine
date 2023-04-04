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
