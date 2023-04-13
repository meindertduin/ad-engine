#include <GL/glew.h>
#include "shader.h"
#include "engine/logging.h"
#include "gpu/gpu.h"

namespace gfx {
    Shader::~Shader() {
        if (compiled()) {
            gpu::destroyShaderProgram(mProgramHandle);
        }
    }

    void Shader::addStage(gfx::ShaderStage&& stage) {
        mStages.push(std::move(stage));
    }

    void Shader::compile() {
        gpu::ShaderHandle vertexShader;
        gpu::ShaderHandle fragmentShader;

        for (auto &stage : mStages) {
            const char* data =  stage.data.data();

            if (stage.type == ShaderType::Vertex) {
                vertexShader = gpu::createShader(gpu::ShaderType::Vertex, data, stage.path.value());
            } else if (stage.type == ShaderType::Fragment) {
                fragmentShader = gpu::createShader(gpu::ShaderType::Fragment, data, stage.path.value());
            }
        }

        mProgramHandle = gpu::createShaderProgram(vertexShader, fragmentShader, true);

        // TODO move to gpu namespace
        auto lightsBlockIndex = glGetUniformBlockIndex(mProgramHandle, "Lights");
        glUniformBlockBinding(mProgramHandle, lightsBlockIndex, LightsBlockBinding);

        bind();
        for (auto const &[name, value] : mUniformLocs) {
            gpu::setUniform(mProgramHandle, name, value);
        }

        mCompiled = true;
    }

    void Shader::bind() const {
        gpu::bindShaderProgram(mProgramHandle);
    }

    void Shader::addUniform(const Uniform &uniform) {
        mUniforms.push(uniform);
    }

    void Shader::addUniformLocs(const std::string &name, int loc) {
        mUniformLocs.try_emplace(name, loc);
    }
}
