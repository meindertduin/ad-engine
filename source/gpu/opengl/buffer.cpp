#include <GL/glew.h>
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

    VertexBuffer::VertexBuffer(const void *data, uint32_t size, const VertexLayout &layout)
        : mLayout(layout)
    {
        glGenVertexArrays(1, &mVertexArrayHandle);
        glGenBuffers(1, &mVertexBufferHandle);

        glBindVertexArray(mVertexArrayHandle);

        glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferHandle);
        glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

        // glEnableVertexAttribArray(0);
        // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

        // glEnableVertexAttribArray(1);
        // glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        bind();
    }

    void VertexBuffer::bind() const {
        glBindVertexArray(mVertexArrayHandle);
        glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferHandle);

        mLayout.bind();
    }

    void VertexBuffer::draw() const {
        glBindVertexArray(mVertexArrayHandle);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    VertexBuffer::~VertexBuffer() {
        glDeleteBuffers(1, &mVertexBufferHandle);
        glDeleteVertexArrays(1, &mVertexArrayHandle);
    }
}