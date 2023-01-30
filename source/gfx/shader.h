#pragma once

#include <string>
#include <vector>
#include <bgfx/bgfx.h>
#include "platform/gcc.h"
#include "engine/path.h"

#include "engine/vector.h"

namespace gfx {
    enum class ShaderType {
        Vertex,
        Fragment
    };

    struct ShaderStage {
        explicit ShaderStage(Allocator &allocator)
            : data(allocator)
        {}

        ShaderType type;
        Path path;
        Vector<char> data;
    };

    struct Uniform {
        std::string name;

        bgfx::UniformHandle handle;
        bgfx::UniformType::Enum type;
    };

    class Shader {
    public:
        explicit Shader(Allocator &allocator)
            : mStages(allocator)
            , mUniforms(allocator)
        {}

        ~Shader();

        void addStage(gfx::ShaderStage&& stage);
        void compile();
        void bind(uint16_t viewId);

        [[nodiscard]] constexpr ALWAYS_INLINE bgfx::ProgramHandle programHandle() const {
            return mProgramHandle;
        }

        [[nodiscard]] constexpr ALWAYS_INLINE bool compiled() const {
            return mCompiled;
        }
    private:
        bgfx::ProgramHandle mProgramHandle = BGFX_INVALID_HANDLE;

        bgfx::ShaderHandle mVertexShaderHandle = BGFX_INVALID_HANDLE;
        bgfx::ShaderHandle mFragmentShaderHandle = BGFX_INVALID_HANDLE;

        bool mCompiled { false };
        bool mDestroyShaders { true };

        Vector<ShaderStage> mStages;
        Vector<Uniform> mUniforms;
    };
}