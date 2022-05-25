#pragma once

#include "LibsHeaders.h"
#include "CppHeaders.h"

#include "Beryll/Renderer/Buffer.h"

namespace Beryll
{
    class AndroidGLESVertexBuffer : public VertexBuffer
    {
    public:
        AndroidGLESVertexBuffer() = delete;
        ~AndroidGLESVertexBuffer() override;

        void bind() override;
        void unBind() override;

    private:
        friend class Renderer;
        // add constructor with proper vector for different buffer
        AndroidGLESVertexBuffer(const std::vector<glm::vec2>& data);
        AndroidGLESVertexBuffer(const std::vector<glm::vec3>& data);
        AndroidGLESVertexBuffer(const std::vector<glm::vec4>& data);
        AndroidGLESVertexBuffer(const std::vector<glm::ivec4>& data);

        uint32_t m_VBO = 0;
    };

    class AndroidGLESIndexBuffer : public IndexBuffer
    {
    public:
        AndroidGLESIndexBuffer() = delete;
        ~AndroidGLESIndexBuffer() override;

        void bind() override;
        void unBind() override;

    private:
        friend class Renderer;
        AndroidGLESIndexBuffer(const std::vector<uint32_t>& indices);

        uint32_t m_EBO = 0;
    };
}