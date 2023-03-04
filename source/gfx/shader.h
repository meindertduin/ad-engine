#pragma once

#include <string>
#include <vector>
#include "platform/gcc.h"
#include "engine/path.h"

#include "engine/vector.h"
#include "engine/resource.h"

#include <unordered_map>

#include <glm/glm.hpp>

namespace gfx {
    class Shader;
    class ShaderManager;

    using ShaderHandle = Handle<Shader>;

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

       enum class Type {
            Float,
            Vec2,
            Vec3,
            Vec4,
            Mat3,
            Mat4
        } type;
    };

    class Shader : public Resource<ShaderManager> {
    public:
        explicit Shader(Allocator &allocator)
            : mStages(allocator)
            , mUniforms(allocator)
        {}

        ~Shader();

        void addStage(gfx::ShaderStage&& stage);
        void compile();
        void bind() const;

        [[nodiscard]] constexpr ALWAYS_INLINE uint32_t programHandle() const {
            return mProgramHandle;
        }

        [[nodiscard]] constexpr ALWAYS_INLINE bool compiled() const {
            return mCompiled;
        }

        [[nodiscard]] constexpr ALWAYS_INLINE const Vector<Uniform>& uniforms() const {
            return mUniforms;
        }

        void addUniform(const Uniform &uniform);
        void addUniformLocs(const std::string &name, int loc);
    private:
        uint32_t mProgramHandle;

        uint32_t mVertexShaderHandle;
        uint32_t mFragmentShaderHandle;

        bool mCompiled { false };
        bool mDestroyShaders { true };

        Vector<ShaderStage> mStages;
        Vector<Uniform> mUniforms;
        std::unordered_map<std::string, int> mUniformLocs;
    };
}