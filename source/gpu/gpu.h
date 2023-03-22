#pragma once

#include <cstdint>
#include <string>
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <functional>
#include "math/size.h"

#include <optional>

namespace gpu {
    enum class ShaderType {
        Vertex,
        Fragment
    };

    enum class Attribute {
        Position,
        Normal,
        TexCoord
    };

    enum class AttributeType {
        Int,
        Float,
        Vec2,
        Vec3,
        Vec4
    };

    class VertexLayout {
    public:
        VertexLayout() = default;

        VertexLayout& addAttribute(Attribute attribute, AttributeType type, bool normalized = false);
        void bind() const;
        [[nodiscard]] constexpr int totalSize() const { return mTotalSize; }
    private:
        int mTotalSize { 0 };

        struct AttributeInfo {
            Attribute attribute;
            AttributeType type;
            uint32_t count;
            uint32_t size;
            bool normalized;
        };

        std::vector<AttributeInfo> mAttributes;
    };

    class VertexBuffer {
    public:
        static std::unique_ptr<VertexBuffer> create(const void *data, uint32_t size, const VertexLayout &layout);
        virtual ~VertexBuffer() = default;

        virtual void bind() const = 0;
        virtual void draw() const = 0;
    };

    class IndexBuffer {
    public:
        static std::unique_ptr<IndexBuffer> create(const uint32_t *data, uint32_t size);
        virtual ~IndexBuffer() = default;

        virtual void bind() const = 0;
    };

    class BufferDataPointer {
    public:
        BufferDataPointer(const void *data, uint32_t size)
            : mData(data)
            , mSize(size)
        {}

        [[nodiscard]] const void* data() const { return mData; }
        [[nodiscard]] uint32_t size() const { return mSize; }
    private:
        const void *mData;
        uint32_t mSize;
    };

    class BufferLayout {
    public:
        struct AttributeInfo {
            std::string name;
            uint32_t offset;
            uint32_t size;
        };

        BufferLayout() = default;

        explicit BufferLayout(uint32_t alignment)
            : mAlignment(alignment)
        {}

        BufferLayout& addAttribute(const std::string &name, uint32_t size);

        [[nodiscard]] uint32_t totalSize() const {
            auto lastOffset = mAttributes.back().offset;
            auto lastSize = mAttributes.back().size;

            auto blockRemainder = (lastOffset + lastSize) % mAlignment;

            auto padding = mAlignment - blockRemainder;

            return lastOffset + lastSize + padding;
        }

        [[nodiscard]] const AttributeInfo& attribute(const std::string &name) const {
            return mAttributes[mAttributeIndices.at(name)];
        }

        [[nodiscard]] const std::vector<AttributeInfo>& attributes() const {
            return mAttributes;
        }
    private:
        uint32_t mAlignment { 16 };

        std::vector<AttributeInfo> mAttributes;
        std::unordered_map<std::string, uint32_t> mAttributeIndices;
    };

    class SharedUniformBuffer {
    public:
        static std::unique_ptr<SharedUniformBuffer> create(uint32_t bindingBlock, uint32_t size);
        virtual ~SharedUniformBuffer() = default;

        virtual void setData(uint32_t offset, void *data, uint32_t size) = 0;

        virtual void bind() const = 0;
    };

    struct DirLight {
        glm::vec3 direction;
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;

        static const BufferLayout* layout() {
            if (sLayout == nullptr) {
                sLayout = std::make_unique<BufferLayout>();

                sLayout->addAttribute("direction", sizeof(glm::vec3));
                sLayout->addAttribute("ambient", sizeof(glm::vec3));
                sLayout->addAttribute("diffuse", sizeof(glm::vec3));
                sLayout->addAttribute("specular", sizeof(glm::vec3));
            }

            return sLayout.get();
        }

        static int bufferSize() {
            return layout()->totalSize();
        }

        void setBufferData(uint32_t startOffset, std::unique_ptr<SharedUniformBuffer> &buffer);
    private:
        static inline std::unique_ptr<BufferLayout> sLayout;
    };


    using ShaderProgramHandle = uint32_t;
    using ShaderHandle = uint32_t;
    using TextureHandle = uint32_t;

    // SHADER

    ShaderHandle createShader(ShaderType type, const char *data, const std::string &shaderName);
    ShaderProgramHandle createShaderProgram(ShaderHandle vertexShader, ShaderHandle fragmentShader, bool destroyShaders);
    void destroyShaderProgram(ShaderProgramHandle handle);
    void bindShaderProgram(ShaderProgramHandle handle);

    void setUniform(ShaderProgramHandle handle, const std::string &name, int value);
    void setUniform(ShaderProgramHandle handle, const std::string &name, float value);
    void setUniform(ShaderProgramHandle handle, const std::string &name, const glm::vec2 &value);
    void setUniform(ShaderProgramHandle handle, const std::string &name, const glm::vec3 &value);
    void setUniform(ShaderProgramHandle handle, const std::string &name, const glm::vec4 &value);
    void setUniform(ShaderProgramHandle handle, const std::string &name, const glm::mat3 &value);
    void setUniform(ShaderProgramHandle handle, const std::string &name, const glm::mat4 &value);

    // TEXTURE
    TextureHandle createTexture2D(unsigned short *data, math::Size2D size, int flags, std::function<void(unsigned short *imageData)> cleanup);
    void destroyTexture(TextureHandle handle);
    void bindTexture(TextureHandle handle, int slot);

    void clear();
    void setViewport(int x, int y, int width, int height);

    bool initialize();
}