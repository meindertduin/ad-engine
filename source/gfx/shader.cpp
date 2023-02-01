#include "shader.h"

namespace gfx {
    Shader::~Shader() {
        if (compiled()) {
            bgfx::destroy(mParamsUniformHandle);
            bgfx::destroy(mProgramHandle);
        }
    }

    void Shader::addStage(gfx::ShaderStage&& stage) {
        mStages.push(std::move(stage));
    }

    void Shader::compile() {
        for (auto &stage : mStages) {
            if (stage.type == ShaderType::Vertex) {
                mVertexShaderHandle = bgfx::createShader(bgfx::makeRef(stage.data.data(), stage.data.size()));
                bgfx::setName(mVertexShaderHandle, stage.path.value().c_str());
            } else if (stage.type == ShaderType::Fragment) {
                mFragmentShaderHandle = bgfx::createShader(bgfx::makeRef(stage.data.data(), stage.data.size()));
                bgfx::setName(mFragmentShaderHandle, stage.path.value().c_str());
            }
        }

        mProgramHandle = bgfx::createProgram(mVertexShaderHandle, mFragmentShaderHandle, mDestroyShaders);
        mParamsUniformHandle = bgfx::createUniform("u_params", bgfx::UniformType::Vec4, 12);

        mCompiled = true;
    }

    void Shader::bind(uint16_t viewId) {
        float params[8] = { 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };

        bgfx::setUniform(mParamsUniformHandle, params, 12);

        bgfx::submit(viewId, mProgramHandle);
    }

    void Shader::addUniform(const Uniform &uniform) {
        mUniforms.push(uniform);
    }
}
