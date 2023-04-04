#include "light.h"

namespace gfx {
    Lights::Lights(const std::vector<DirLight> &dirLights)
            : mDirLightsCount(static_cast<int>(dirLights.size()))
            , mDirLights(dirLights)
    {
        mLayout = std::make_unique<gpu::BufferLayout>();
        mLayout->addAttribute("dirLightsCount", sizeof(int));

        for (int i = 0; i < mDirLightsCount; ++i) {
            mLayout->addAttribute("dirLights[" + std::to_string(i) + "].direction", sizeof(glm::vec3));
            mLayout->addAttribute("dirLights[" + std::to_string(i) + "].ambient", sizeof(glm::vec3));
            mLayout->addAttribute("dirLights[" + std::to_string(i) + "].diffuse", sizeof(glm::vec3));
            mLayout->addAttribute("dirLights[" + std::to_string(i) + "].specular", sizeof(glm::vec3));
        }

        mBuffer = gpu::SharedUniformBuffer::create(LightsBlockBinding, mLayout->totalSize());
    }

    void Lights::setBufferData() {
        auto dirLightsCountOffset = mLayout->attribute("dirLightsCount").offset;

        mBuffer->setData(dirLightsCountOffset, &mDirLightsCount, sizeof(int));

        for (int i = 0; i < mDirLightsCount; i++) {
            auto directionOffset = mLayout->attribute("dirLights[" + std::to_string(i) + "].direction").offset;
            auto ambientOffset = mLayout->attribute("dirLights[" + std::to_string(i) + "].ambient").offset;
            auto diffuseOffset = mLayout->attribute("dirLights[" + std::to_string(i) + "].diffuse").offset;
            auto specularOffset = mLayout->attribute("dirLights[" + std::to_string(i) + "].specular").offset;

            mBuffer->setData(directionOffset, &mDirLights[i].direction, sizeof(glm::vec3));
            mBuffer->setData(ambientOffset, &mDirLights[i].ambient, sizeof(glm::vec3));
            mBuffer->setData(diffuseOffset, &mDirLights[i].diffuse, sizeof(glm::vec3));
            mBuffer->setData(specularOffset, &mDirLights[i].specular, sizeof(glm::vec3));
        }
    }
}