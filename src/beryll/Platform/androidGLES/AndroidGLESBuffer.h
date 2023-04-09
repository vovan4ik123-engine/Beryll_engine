#pragma once

#include "beryll/renderer/Buffer.h"
#include "beryll/core/Log.h"

namespace Beryll
{
    class AndroidGLESStaticVertexBuffer : public VertexBuffer
    {
    public:
        AndroidGLESStaticVertexBuffer() = delete;
        ~AndroidGLESStaticVertexBuffer() override;

        void bind() override; // Must be called only inside VAO
        void unBind() override; // Must be called only inside VAO

        void setDynamicBufferData(const std::vector<glm::vec3>& data, uint32_t elementsCount) override
        {
            BR_ASSERT(false, "%s", "Can not set data into static vertex buffer");
        };
        void setDynamicBufferData(const std::vector<glm::vec4>& data, uint32_t elementsCount) override
        {
            BR_ASSERT(false, "%s", "Can not set data into static vertex buffer");
        };
        void setDynamicBufferData(const std::vector<glm::mat4>& data, uint32_t elementsCount) override
        {
            BR_ASSERT(false, "%s", "Can not set data into static vertex buffer");
        };

    private:
        friend class Renderer;
        // add constructor with proper vector for different buffer
        AndroidGLESStaticVertexBuffer(const std::vector<glm::vec2>& data);
        AndroidGLESStaticVertexBuffer(const std::vector<glm::vec3>& data);
        AndroidGLESStaticVertexBuffer(const std::vector<glm::vec4>& data);
        AndroidGLESStaticVertexBuffer(const std::vector<glm::ivec4>& data);
        AndroidGLESStaticVertexBuffer(const std::vector<glm::mat4>& data);

        uint32_t m_VBO = 0;
    };

    class AndroidGLESDynamicVertexBuffer : public VertexBuffer
    {
    public:
        AndroidGLESDynamicVertexBuffer() = delete;
        ~AndroidGLESDynamicVertexBuffer() override;

        void bind() override; // Must be called only inside VAO
        void unBind() override; // Must be called only inside VAO

        // Only for dynamic buffer
        // elementsCount = data.size() for copy all buffer
        void setDynamicBufferData(const std::vector<glm::vec3>& data, uint32_t elementsCount) override;
        void setDynamicBufferData(const std::vector<glm::vec4>& data, uint32_t elementsCount) override;
        void setDynamicBufferData(const std::vector<glm::mat4>& data, uint32_t elementsCount) override;

    private:
        friend class Renderer;
        AndroidGLESDynamicVertexBuffer(VertexAttribType type, VertexAttribSize size, uint32_t maxSizeBytes);

        uint32_t m_VBO = 0;
        const uint32_t m_originalSizeBytes = 0;
    };

    // Dynamic index buffer can be achieved by changing m_count inside setCount(uint32_t count)
    class AndroidGLESStaticIndexBuffer : public IndexBuffer
    {
    public:
        AndroidGLESStaticIndexBuffer() = delete;
        ~AndroidGLESStaticIndexBuffer() override;

        void bind() override;
        void unBind() override;

        void setCount(uint32_t count) override;
    private:
        friend class Renderer;
        AndroidGLESStaticIndexBuffer(const std::vector<uint32_t>& indices);

        uint32_t m_EBO = 0;
        const uint32_t m_originalCount = 0;
    };
}