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

    void clear() {
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void setViewport(int x, int y, int width, int height) {
        glViewport(x, y, width, height);
    }

    bool initialize() {
        glewExperimental = GL_TRUE;
        GLenum glewError = glewInit();

        if(glewError != GLEW_OK) {
            Logger::error("Failed to initialize GLEW: {}", (const char*)glewGetErrorString(glewError));
            return false;
        }

        return true;
    }

    TextureHandle createTexture2D(unsigned short *data, math::Size2D size, int flags, std::function<void(unsigned short *imageData)> cleanup) {
        TextureHandle texture { 0 };
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.width(), size.height(), 0, GL_RGBA, GL_UNSIGNED_SHORT, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        cleanup(data);

        return texture;
    }

    void destroyTexture(TextureHandle handle) {
        glDeleteTextures(1, &handle);
    }

    void bindTexture(TextureHandle handle, int slot) {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, handle);
    }

    BufferLayout &BufferLayout::addAttribute(const std::string &name, uint32_t size) {
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

        mAttributes.push_back({ name, newOffset, size });

        auto index = mAttributes.size() - 1;
        mAttributeIndices[name] = index;

        return *this;
    }

    void BufferLayout::padLastAttribute() {
        auto &lastAttribute = mAttributes.back();
        auto blockRemainder = (lastAttribute.offset + lastAttribute.size) % mAlignment;
        if (blockRemainder != 0) {
            lastAttribute.size += mAlignment - blockRemainder;
        }
    }
}