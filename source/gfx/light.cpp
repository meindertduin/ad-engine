#include "light.h"

namespace gfx {
    Lights::Lights(const std::vector<DirLight> &dirLights, const std::vector<PointLight> &pointLights)
            : mDirLightsCount(static_cast<int>(dirLights.size()))
            , mDirLights(dirLights)
            , mPointLightsCount(static_cast<int>(pointLights.size()))
            , mPointLights(pointLights)
    {
        mLayout = std::make_unique<gpu::BufferLayout>();
        mLayout->addAttribute("dirLightsCount", sizeof(int));
        mLayout->padLastAttribute();

        for (int i = 0; i < MaxDirLights; i++) {
            mLayout->addAttribute("dirLights[" + std::to_string(i) + "].direction", sizeof(glm::vec3));
            mLayout->addAttribute("dirLights[" + std::to_string(i) + "].ambient", sizeof(glm::vec3));
            mLayout->addAttribute("dirLights[" + std::to_string(i) + "].diffuse", sizeof(glm::vec3));
            mLayout->addAttribute("dirLights[" + std::to_string(i) + "].specular", sizeof(glm::vec3));

            mLayout->padLastAttribute();
        }

        mLayout->addAttribute("pointLightsCount", sizeof(int));

        for (int i = 0; i < MaxPointLights; i++) {
            mLayout->addAttribute("pointLights[" + std::to_string(i) + "].position", sizeof(glm::vec3));
            mLayout->addAttribute("pointLights[" + std::to_string(i) + "].ambient", sizeof(glm::vec3));
            mLayout->addAttribute("pointLights[" + std::to_string(i) + "].diffuse", sizeof(glm::vec3));
            mLayout->addAttribute("pointLights[" + std::to_string(i) + "].specular", sizeof(glm::vec3));
            mLayout->addAttribute("pointLights[" + std::to_string(i) + "].constant", sizeof(float));
            mLayout->addAttribute("pointLights[" + std::to_string(i) + "].linear", sizeof(float));
            mLayout->addAttribute("pointLights[" + std::to_string(i) + "].quadratic", sizeof(float));

            mLayout->padLastAttribute();
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

        auto pointLightsCountOffset = mLayout->attribute("pointLightsCount").offset;
        mBuffer->setData(pointLightsCountOffset, &mPointLightsCount, sizeof(int));

        for (int i = 0; i < mPointLightsCount; i++) {
            auto positionOffset = mLayout->attribute("pointLights[" + std::to_string(i) + "].position").offset;
            auto ambientOffset = mLayout->attribute("pointLights[" + std::to_string(i) + "].ambient").offset;
            auto diffuseOffset = mLayout->attribute("pointLights[" + std::to_string(i) + "].diffuse").offset;
            auto specularOffset = mLayout->attribute("pointLights[" + std::to_string(i) + "].specular").offset;
            auto constantOffset = mLayout->attribute("pointLights[" + std::to_string(i) + "].constant").offset;
            auto linearOffset = mLayout->attribute("pointLights[" + std::to_string(i) + "].linear").offset;
            auto quadraticOffset = mLayout->attribute("pointLights[" + std::to_string(i) + "].quadratic").offset;

            mBuffer->setData(positionOffset, &mPointLights[i].position, sizeof(glm::vec3));
            mBuffer->setData(ambientOffset, &mPointLights[i].ambient, sizeof(glm::vec3));
            mBuffer->setData(diffuseOffset, &mPointLights[i].diffuse, sizeof(glm::vec3));
            mBuffer->setData(specularOffset, &mPointLights[i].specular, sizeof(glm::vec3));
            mBuffer->setData(constantOffset, &mPointLights[i].constant, sizeof(float));
            mBuffer->setData(linearOffset, &mPointLights[i].linear, sizeof(float));
            mBuffer->setData(quadraticOffset, &mPointLights[i].quadratic, sizeof(float));
        }
    }
}