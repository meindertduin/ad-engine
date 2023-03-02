#include <GL/glew.h>
#include "shader.h"
#include "engine/logging.h"

namespace gfx {
    Shader::~Shader() {
        if (compiled()) {
            glDeleteShader(mVertexShaderHandle);
            glDeleteShader(mFragmentShaderHandle);
            glDeleteProgram(mProgramHandle);
        }
    }

    void Shader::addStage(gfx::ShaderStage&& stage) {
        mStages.push(std::move(stage));
    }

    void Shader::compile() {
        for (auto &stage : mStages) {
            auto data = reinterpret_cast<const GLchar *const *>(stage.data.data());

            int success;
            char infoLog[512];
            if (stage.type == ShaderType::Vertex) {
                mVertexShaderHandle = glCreateShader(GL_VERTEX_SHADER);
                glShaderSource(mVertexShaderHandle, 1, data, nullptr);
                glCompileShader(mVertexShaderHandle);

                glGetShaderiv(mVertexShaderHandle, GL_COMPILE_STATUS, &success);
                if (!success)
                {
                    glGetShaderInfoLog(mVertexShaderHandle, 512, nullptr, infoLog);
                    Logger::error("ERROR::SHADER::VERTEX::COMPILATION_FAILED");
                }
            } else if (stage.type == ShaderType::Fragment) {
                mFragmentShaderHandle = glCreateShader(GL_FRAGMENT_SHADER);
                glShaderSource(mFragmentShaderHandle, 1, data, nullptr);
                glCompileShader(mFragmentShaderHandle);

                glGetShaderiv(mFragmentShaderHandle, GL_COMPILE_STATUS, &success);
                if (!success)
                {
                    glGetShaderInfoLog(mFragmentShaderHandle, 512, nullptr, infoLog);
                    Logger::error("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED");
                }
            }
        }

        mProgramHandle = glCreateProgram();
        glAttachShader(mProgramHandle, mVertexShaderHandle);
        glAttachShader(mProgramHandle, mFragmentShaderHandle);
        glLinkProgram(mProgramHandle);

        int success;
        char infoLog[512];

        glGetProgramiv(mProgramHandle, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(mProgramHandle, 512, NULL, infoLog);
            Logger::error("ERROR::SHADER::PROGRAM::LINKING_FAILED");
        }

        mCompiled = true;
    }

    void Shader::bind() const {
        glUseProgram(mProgramHandle);
    }

    void Shader::addUniform(const Uniform &uniform) {
        mUniforms.push(uniform);
    }
}
