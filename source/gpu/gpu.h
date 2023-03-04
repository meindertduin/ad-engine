#pragma once

#include <cstdint>
#include <string>
#include <glm/glm.hpp>

namespace gpu {
    enum class ShaderType {
        Vertex,
        Fragment
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