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
        VertexBufferImpl(const void *data, uint32_t totalSize, VertexLayout layout)
            : mVertexCount(totalSize / layout.totalSize())
            , mLayout(std::move(layout))
        {
            glGenVertexArrays(1, &mVertexArrayHandle);
            glGenBuffers(1, &mVertexBufferHandle);

            glBindVertexArray(mVertexArrayHandle);

            glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferHandle);
            glBufferData(GL_ARRAY_BUFFER, totalSize, data, GL_STATIC_DRAW);

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
            glDrawArrays(GL_TRIANGLES, 0, mVertexCount);
        }

    private:
        uint32_t mVertexArrayHandle { 0 };
        uint32_t mVertexBufferHandle { 0 };
        uint32_t mVertexCount { 0 };
        VertexLayout mLayout;
    };

    std::unique_ptr<VertexBuffer> VertexBuffer::create(const void *data, uint32_t size, const VertexLayout &layout) {
        return std::make_unique<VertexBufferImpl>(data, size, layout);
    }

    class IndexBufferImpl : public IndexBuffer {
    public:
        IndexBufferImpl(const uint32_t *data, uint32_t size)
        {
            glGenBuffers(1, &mIndexBufferHandle);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferHandle);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
        }

        ~IndexBufferImpl() override {
            glDeleteBuffers(1, &mIndexBufferHandle);
        }

        IndexBufferImpl(const IndexBufferImpl&) = delete;
        IndexBufferImpl& operator=(const IndexBufferImpl&) = delete;

        void bind() const override {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferHandle);
        }
    private:
        uint32_t mIndexBufferHandle { 0 };
    };

    std::unique_ptr<IndexBuffer> IndexBuffer::create(const uint32_t *data, uint32_t size) {
        return std::make_unique<IndexBufferImpl>(data, size);
    }

    class SharedUniformBufferImpl : public SharedUniformBuffer {
    public:
        SharedUniformBufferImpl(uint32_t bindingBlock, uint32_t size)
            : mSize(size)
        {
            glGenBuffers(1, &mUniformBufferHandle);
            glBindBuffer(GL_UNIFORM_BUFFER, mUniformBufferHandle);
            glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);

            glBindBufferRange(GL_UNIFORM_BUFFER, bindingBlock, mUniformBufferHandle, 0, size);
        }

        ~SharedUniformBufferImpl() override {
            glDeleteBuffers(1, &mUniformBufferHandle);
        }

        void setData(uint32_t offset, void *data, uint32_t size) override {
            bind();
            glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
        }

        void bind() const override {
            glBindBuffer(GL_UNIFORM_BUFFER, mUniformBufferHandle);
        }

    private:
        uint32_t mUniformBufferHandle { 0 };
        uint32_t mSize { 0 };
    };

    void DirLight::setBufferData(uint32_t startOffset, std::unique_ptr<SharedUniformBuffer> &buffer) {
        auto layout = DirLight::layout();

        auto directionOffset = layout->attribute("direction").offset + startOffset;
        auto ambientOffset = layout->attribute("ambient").offset + startOffset;
        auto diffuseOffset = layout->attribute("diffuse").offset + startOffset;
        auto specularOffset = layout->attribute("specular").offset + startOffset;

        buffer->setData(directionOffset, &direction, sizeof(direction));
        buffer->setData(ambientOffset, &ambient, sizeof(ambient));
        buffer->setData(diffuseOffset, &diffuse, sizeof(diffuse));
        buffer->setData(specularOffset, &specular, sizeof(specular));
    }

    std::unique_ptr<SharedUniformBuffer> SharedUniformBuffer::create(uint32_t bindingBlock, uint32_t size) {
        return std::make_unique<SharedUniformBufferImpl>(bindingBlock, size);
    }
}