#include "Renderer.h"
#include "Beryll/Core/Log.h"
#include "Beryll/Core/GameLoop.h"

#if defined(ANDROID)
    #include "Beryll/Platform/AndroidGLES/AndroidGLESBuffer.h"
    #include "Beryll/Platform/AndroidGLES/AndroidGLESVertexArray.h"
    #include "Beryll/Platform/AndroidGLES/AndroidGLESShader.h"
    #include "Beryll/Platform/AndroidGLES/AndroidGLESTexture.h"
    #include "Beryll/Platform/AndroidGLES/AndroidGLESShadowMapTexture.h"
    #include "Beryll/Platform/AndroidGLES/AndroidGLESSkyBox.h"
#elif defined(APPLE)

#endif

namespace Beryll
{
    std::shared_ptr<VertexBuffer> Renderer::createVertexBuffer(const std::vector<glm::vec2>& data)
    {
#if defined(ANDROID)
        return std::shared_ptr<VertexBuffer>(new AndroidGLESVertexBuffer(data));
#elif defined(APPLE)

#else
        BR_ASSERT(false, "%s", "Can not create VertexBuffer. Unknown platform.");
        return nullptr;
#endif
    }

    std::shared_ptr<VertexBuffer> Renderer::createVertexBuffer(const std::vector<glm::vec3>& data)
    {
#if defined(ANDROID)
        return std::shared_ptr<VertexBuffer>(new AndroidGLESVertexBuffer(data));
#elif defined(APPLE)

#else
        BR_ASSERT(false, "%s", "Can not create VertexBuffer. Unknown platform.");
        return nullptr;
#endif
    }

    std::shared_ptr<VertexBuffer> Renderer::createVertexBuffer(const std::vector<glm::vec4>& data)
    {
#if defined(ANDROID)
        return std::shared_ptr<VertexBuffer>(new AndroidGLESVertexBuffer(data));
#elif defined(APPLE)

#else
        BR_ASSERT(false, "%s", "Can not create VertexBuffer. Unknown platform.");
        return nullptr;
#endif
    }

    std::shared_ptr<VertexBuffer> Renderer::createVertexBuffer(const std::vector<glm::ivec4>& data)
    {
#if defined(ANDROID)
        return std::shared_ptr<VertexBuffer>(new AndroidGLESVertexBuffer(data));
#elif defined(APPLE)

#else
        BR_ASSERT(false, "%s", "Can not create VertexBuffer. Unknown platform.");
        return nullptr;
#endif
    }

    std::shared_ptr<IndexBuffer> Renderer::createIndexBuffer(const std::vector<uint32_t>& indices)
    {
#if defined(ANDROID)
        return std::shared_ptr<IndexBuffer>(new AndroidGLESIndexBuffer(indices));
#elif defined(APPLE)

#else
        BR_ASSERT(false, "%s", "Can not create IndexBuffer. Unknown platform.");
        return nullptr;
#endif
    }

    std::unique_ptr<VertexArray> Renderer::createVertexArray()
    {
#if defined(ANDROID)
        return std::unique_ptr<VertexArray>(new AndroidGLESVertexArray());
#elif defined(APPLE)

#else
        BR_ASSERT(false, "%s", "Can not create VertexArray. Unknown platform.");
        return nullptr;
#endif
    }

    std::shared_ptr<Shader> Renderer::createShader(const char* vertexPath, const char* fragmentPath)
    {
#if defined(ANDROID)
        return std::shared_ptr<Shader>(new AndroidGLESShader(vertexPath, fragmentPath));
#elif defined(APPLE)

#else
        BR_ASSERT(false, "%s", "Can not create Shader. Unknown platform.");
        return nullptr;
#endif
    }

    std::unique_ptr<Texture> Renderer::createTexture(const char* path, TextureType type)
    {
#if defined(ANDROID)
        return std::unique_ptr<Texture>(new AndroidGLESTexture(path, type));
#elif defined(APPLE)

#else
        BR_ASSERT(false, "%s", "Can not create Texture. Unknown platform.");
        return nullptr;
#endif
    }

    std::unique_ptr<Texture> Renderer::createShadowMapTexture(int width, int height)
    {
#if defined(ANDROID)
        return std::unique_ptr<Texture>(new AndroidGLESShadowMapTexture(width, height));
#elif defined(APPLE)

#else
        BR_ASSERT(false, "%s", "Can not create shadow map Texture. Unknown platform.");
        return nullptr;
#endif
    }

    std::unique_ptr<SkyBox> Renderer::createSkyBox(const char* folderPath)
    {
#if defined(ANDROID)
        return std::unique_ptr<SkyBox>(new AndroidGLESSkyBox(folderPath));
#elif defined(APPLE)

#else
        BR_ASSERT(false, "%s", "Can not create SkyBox. Unknown platform.");
        return nullptr;
#endif
    }
}
