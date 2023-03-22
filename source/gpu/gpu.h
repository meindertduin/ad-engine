#pragma once

#include <cstdint>
#include <string>
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <functional>
#include "math/size.h"

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

    class SharedBufferLayout {
    public:
        struct AttributeInfo {
            uint32_t offset;
            AttributeType type;
            std::string name;
            uint32_t size;
        };

        SharedBufferLayout() = default;

        explicit SharedBufferLayout(uint32_t alignment)
            : mAlignment(alignment)
        {}

        SharedBufferLayout& addAttribute(const std::string &name, AttributeType type, uint32_t size) {
            uint32_t lastOffset;
            uint32_t lastSize;
            if (mAttributes.empty()) {
                lastOffset = 0;
                lastSize = 0;
            } else {
                lastOffset = mAttributes.back().offset;
                lastSize = mAttributes.back().size;
            }

            auto newOffset = lastOffset + lastSize;
            auto blockRemainder = newOffset % mAlignment;

            // Add padding to previous block
            if (blockRemainder != 0 && blockRemainder < size) {
                newOffset += mAlignment - blockRemainder;
            }

            mAttributes.push_back({ newOffset, type, name, size });

            auto index = mAttributes.size() - 1;
            mAttributeIndices[name] = index;

            return *this;
        }

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
        static std::unique_ptr<SharedUniformBuffer> create(uint32_t bindingBlock, const SharedBufferLayout &layout);
        virtual ~SharedUniformBuffer() = default;

        virtual void setData(const std::string &name, const BufferDataPointer &data) = 0;

        virtual void bind() const = 0;
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