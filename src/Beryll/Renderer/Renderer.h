#pragma once

#include "LibsHeaders.h"
#include "CppHeaders.h"

#include "Buffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"

namespace Beryll
{
    // static methods for create things required for rendering objects
    class Renderer final
    {
    public:
        Renderer() = delete;
        ~Renderer() = delete;

        static std::shared_ptr<VertexBuffer> createVertexBuffer(const std::vector<glm::vec2>& data);
        static std::shared_ptr<VertexBuffer> createVertexBuffer(const std::vector<glm::vec3>& data);
        static std::shared_ptr<VertexBuffer> createVertexBuffer(const std::vector<glm::vec4>& data);
        static std::shared_ptr<VertexBuffer> createVertexBuffer(const std::vector<glm::ivec4>& data);

        static std::shared_ptr<IndexBuffer> createIndexBuffer(const std::vector<uint32_t>& indices);

        static std::unique_ptr<VertexArray> createVertexArray();

        static std::shared_ptr<Shader> createShader(const char* vertexPath, const char* fragmentPath);

        static std::unique_ptr<Texture> createTexture(const char* path, TextureType type);
    };
}
