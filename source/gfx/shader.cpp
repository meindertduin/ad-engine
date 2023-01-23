#include "shader.h"

namespace gfx {
    Shader::~Shader() {
        bgfx::destroy(mProgramHandle);
        if (mDestroyShaders) {
            bgfx::destroy(mVertexShaderHandle);
            bgfx::destroy(mFragmentShaderHandle);
        }
    }

    void Shader::addStage(const gfx::ShaderStage& stage) {
        mStages.push_back(stage);
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

        mProgramHandle = bgfx::createProgram(mVertexShaderHandle, mFragmentShaderHandle, true);
        mCompiled = true;
    }

    void Shader::bind(uint16_t viewId) {
        bgfx::submit(viewId, mProgramHandle);
    }
}