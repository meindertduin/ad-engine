#pragma once

#include <string>
#include <vector>
#include <bgfx/bgfx.h>
#include "platform/gcc.h"
#include "engine/path.h"

namespace gfx {
    enum class ShaderType {
        Vertex,
        Fragment
    };

    struct ShaderStage {
        ShaderType type;
        Path path;
        std::string data;
    };

    class Shader {
    public:
        ~Shader();

        void addStage(const gfx::ShaderStage& stage);
        void compile();
        void bind(uint16_t viewId);

        [[nodiscard]] constexpr ALWAYS_INLINE bgfx::ProgramHandle programHandle() const {
            return mProgramHandle;
        }

        [[nodiscard]] constexpr ALWAYS_INLINE bool isCompiled() const {
            return mCompiled;
        }
    private:
        bgfx::ProgramHandle mProgramHandle = BGFX_INVALID_HANDLE;

        bgfx::ShaderHandle mVertexShaderHandle = BGFX_INVALID_HANDLE;
        bgfx::ShaderHandle mFragmentShaderHandle = BGFX_INVALID_HANDLE;

        bool mCompiled;
        bool mDestroyShaders { true };

        std::vector<ShaderStage> mStages;
    };
}