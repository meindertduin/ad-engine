#include <GL/glew.h>

#include <utility>
#include "gpu/gpu.h"

namespace gpu {
    VertexLayout& VertexLayout::addAttribute(Attribute attribute, AttributeType type, bool normalized) {
        uint32_t size = 0;
        uint32_t count = 0;
        switch (type) {
            using enum gpu::AttributeType;
            case Float:
                size = sizeof(float);
                count = 1;
                break;
            case Vec2:
                size = sizeof(glm::vec2);
                count = 2;
                break;
            case Vec3:
                size = sizeof(glm::vec3);
                count = 3;
                break;
            case Vec4:
                size = sizeof(glm::vec4);
                count = 4;
                break;
        }

        mTotalSize += size;
        mAttributes.push_back({attribute, type, count, size, normalized});

        return *this;
    }

    void VertexLayout::bind() const {
        using enum gpu::AttributeType;

        uint32_t offset = 0;
        int index = 0;
        for (const auto &attribute : mAttributes) {
            glEnableVertexAttribArray(index);
            glVertexAttribPointer(index, attribute.count, GL_FLOAT, attribute.normalized, mTotalSize, (void*) offset);

            offset += attribute.size;
            index++;
        }
    }

    class VertexBufferImpl : public VertexBuffer {
    public:
        VertexBufferImpl(const void *data, uint32_t size, VertexLayout layout)
            : mLayout(std::move(layout))
        {
            glGenVertexArrays(1, &mVertexArrayHandle);
            glGenBuffers(1, &mVertexBufferHandle);

            glBindVertexArray(mVertexArrayHandle);

            glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferHandle);
            glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

            mLayout.bind();
        }

        ~VertexBufferImpl() override {
            glDeleteBuffers(1, &mVertexBufferHandle);
            glDeleteVertexArrays(1, &mVertexArrayHandle);
        }

        VertexBufferImpl(const VertexBufferImpl&) = delete;
        VertexBufferImpl& operator=(const VertexBufferImpl&) = delete;

        void bind() const override {
            glBindVertexArray(mVertexArrayHandle);
            glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferHandle);

            mLayout.bind();
        }

        void draw() const override {
            glBindVertexArray(mVertexArrayHandle);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }

    private:
        uint32_t mVertexArrayHandle { 0 };
        uint32_t mVertexBufferHandle { 0 };
        VertexLayout mLayout;
    };

    std::unique_ptr<VertexBuffer> VertexBuffer::create(const void *data, uint32_t size, const VertexLayout &layout) {
        return std::make_unique<VertexBufferImpl>(data, size, layout);
    }
}