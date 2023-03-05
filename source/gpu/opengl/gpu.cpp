#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include "gpu/gpu.h"
#include "engine/logging.h"

namespace gpu {
    ShaderHandle createShader(ShaderType type, const char *data, const std::string &shaderName) {
        int success;
        char infoLog[512];

        auto glShaderType = type == ShaderType::Vertex ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER;
        ShaderHandle shader = glCreateShader(glShaderType);
        glShaderSource(shader, 1, &data, nullptr);
        glCompileShader(shader);

        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 512, nullptr, infoLog);
            Logger::error("Compilation of shader {} failed: {}", shaderName, infoLog);
        }

        return shader;
    }

    ShaderProgramHandle createShaderProgram(ShaderHandle vertexShader, ShaderHandle fragmentShader, bool destroyShaders) {
        int success;
        char infoLog[512];

        ShaderProgramHandle program = glCreateProgram();
        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);
        glLinkProgram(program);

        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(program, 512, nullptr, infoLog);
            Logger::error("Linking of shader program failed: {}", infoLog);
        }

        if (destroyShaders) {
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
        }

        return program;
    }

    void destroyShaderProgram(ShaderProgramHandle handle) {
        glDeleteProgram(handle);
    }

    void bindShaderProgram(ShaderProgramHandle handle) {
        glUseProgram(handle);
    }

    void setUniform(ShaderProgramHandle handle, const std::string &name, int value) {
        auto uniformLocation = glGetUniformLocation(handle, name.c_str());
        if (uniformLocation == -1) {
            Logger::error("Uniform not found: {}", name);
        }
        glUniform1i(uniformLocation, value);
    }

    void setUniform(ShaderProgramHandle handle, const std::string &name, float value) {
        auto uniformLocation = glGetUniformLocation(handle, name.c_str());
        if (uniformLocation == -1) {
            Logger::error("Uniform not found: {}", name);
        }
        glUniform1f(uniformLocation, value);
    }

    void setUniform(ShaderProgramHandle handle, const std::string &name, const glm::vec2 &value) {
        auto uniformLocation = glGetUniformLocation(handle, name.c_str());
        if (uniformLocation == -1) {
            Logger::error("Uniform not found: {}", name);
        }
        glUniform2f(uniformLocation, value.x, value.y);
    }

    void setUniform(ShaderProgramHandle handle, const std::string &name, const glm::vec3 &value) {
        auto uniformLocation = glGetUniformLocation(handle, name.c_str());
        if (uniformLocation == -1) {
            Logger::error("Uniform not found: {}", name);
        }
        glUniform3f(uniformLocation, value.x, value.y, value.z);
    }

    void setUniform(ShaderProgramHandle handle, const std::string &name, const glm::vec4 &value) {
        auto uniformLocation = glGetUniformLocation(handle, name.c_str());
        if (uniformLocation == -1) {
            Logger::error("Uniform not found: {}", name);
        }
        glUniform4f(uniformLocation, value.x, value.y, value.z, value.w);
    }
    void setUniform(ShaderProgramHandle handle, const std::string &name, const glm::mat3 &value) {
        auto uniformLocation = glGetUniformLocation(handle, name.c_str());
        if (uniformLocation == -1) {
            Logger::error("Uniform not found: {}", name);
        }
        glUniformMatrix3fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(value));
    }

    void setUniform(ShaderProgramHandle handle, const std::string &name, const glm::mat4 &value) {
        auto uniformLocation = glGetUniformLocation(handle, name.c_str());
        if (uniformLocation == -1) {
            Logger::error("Uniform not found: {}", name);
        }
        glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(value));
    }

}