#pragma once

#include "LibsHeaders.h"
#include "CppHeaders.h"

#include "Buffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"
#include "ShadowMap.h"
#include "SkyBox.h"

namespace Beryll
{
    // Forward declaration.
    class BaseSimpleObject;
    class BaseAnimatedObject;

    // Static methods for create things required for rendering objects.
    class Renderer final
    {
    public:
        Renderer() = delete;
        ~Renderer() = delete;

        static std::shared_ptr<VertexBuffer> createStaticVertexBuffer(const std::vector<glm::vec2>& data);
        static std::shared_ptr<VertexBuffer> createStaticVertexBuffer(const std::vector<glm::vec3>& data);
        static std::shared_ptr<VertexBuffer> createStaticVertexBuffer(const std::vector<glm::vec4>& data);
        static std::shared_ptr<VertexBuffer> createStaticVertexBuffer(const std::vector<glm::ivec4>& data);
        static std::shared_ptr<VertexBuffer> createStaticVertexBuffer(const std::vector<glm::mat4>& data);

        static std::shared_ptr<VertexBuffer> createDynamicVertexBuffer(VertexAttribType type, VertexAttribSize size, uint32_t maxSizeBytes);

        static std::shared_ptr<IndexBuffer> createStaticIndexBuffer(const std::vector<uint32_t>& indices);
        // If you wand dynamic index buffer: create static index buffer with max possible indices
        //                                   and change count by setCount(uint32_t count) every frame.

        static std::unique_ptr<VertexArray> createVertexArray();

        static std::shared_ptr<Shader> createShader(const char* vertexPath, const char* fragmentPath);
        static std::unique_ptr<Texture> createTexture(const char* path, TextureType type);
        static std::unique_ptr<ShadowMap> createShadowMap(int width, int height);

        static std::unique_ptr<SkyBox> createSkyBox(const char* folderPath);

        // These 2 methods will set BaseSimpleObject and BaseAnimatedObject specific uniform variables
        // which shader must have to draw them.
        // If shader has some extra uniform variables they should be set before this methods call.
        static void drawObject(const std::shared_ptr<Beryll::BaseSimpleObject>& obj,
                               const glm::mat4& modelMatrix,
                               const std::shared_ptr<Shader>& shader = nullptr);
        static void drawObject(const std::shared_ptr<Beryll::BaseAnimatedObject>& obj,
                               const glm::mat4& modelMatrix,
                               const std::shared_ptr<Shader>& shader = nullptr);

        static void enableFaceCulling();
        static void disableFaceCulling();
        static void setDepthFunctionLess();
        static void setDepthFunctionAlways();
    };
}
