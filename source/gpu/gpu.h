#pragma once

#include <cstdint>
#include <string>
#include <glm/glm.hpp>
#include <vector>

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
        VertexBuffer(const void *data, uint32_t size, const VertexLayout &layout);
        ~VertexBuffer();

        VertexBuffer(const VertexBuffer&) = delete;
        VertexBuffer& operator=(const VertexBuffer&) = delete;

        void bind() const;
        void draw() const;
    private:
        uint32_t mVertexArrayHandle { 0 };
        uint32_t mVertexBufferHandle { 0 };
        VertexLayout mLayout;
    };

    using ShaderProgramHandle = uint32_t;
    using ShaderHandle = uint32_t;

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
}