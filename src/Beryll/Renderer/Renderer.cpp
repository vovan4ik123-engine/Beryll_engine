#include "Renderer.h"
#include "Beryll/Core/Log.h"
#include "Beryll/Core/GameLoop.h"

#include "Beryll/Platform/AndroidGLES/AndroidGLESBuffer.h"
#include "Beryll/Platform/AndroidGLES/AndroidGLESVertexArray.h"
#include "Beryll/Platform/AndroidGLES/AndroidGLESShader.h"
#include "Beryll/Platform/AndroidGLES/AndroidGLESTexture.h"

namespace Beryll
{
    std::shared_ptr<VertexBuffer> Renderer::createVertexBuffer(const std::vector<glm::vec2>& data)
    {
        if(GameLoop::getPlatform() == Platform::ANDROID_GLES)
        {
            return std::shared_ptr<VertexBuffer>(new AndroidGLESVertexBuffer(data));
        }
        else
        {
            BR_ASSERT(false, "%s", "Can not create VertexBuffer. Unknown platform.");
            return nullptr;
        }
    }

    std::shared_ptr<VertexBuffer> Renderer::createVertexBuffer(const std::vector<glm::vec3>& data)
    {
        if(GameLoop::getPlatform() == Platform::ANDROID_GLES)
        {
            return std::shared_ptr<VertexBuffer>(new AndroidGLESVertexBuffer(data));
        }
        else
        {
            BR_ASSERT(false, "%s", "Can not create VertexBuffer. Unknown platform.");
            return nullptr;
        }
    }

    std::shared_ptr<VertexBuffer> Renderer::createVertexBuffer(const std::vector<glm::vec4>& data)
    {
        if(GameLoop::getPlatform() == Platform::ANDROID_GLES)
        {
            return std::shared_ptr<VertexBuffer>(new AndroidGLESVertexBuffer(data));
        }
        else
        {
            BR_ASSERT(false, "%s", "Can not create VertexBuffer. Unknown platform.");
            return nullptr;
        }
    }

    std::shared_ptr<VertexBuffer> Renderer::createVertexBuffer(const std::vector<glm::ivec4>& data)
    {
        if(GameLoop::getPlatform() == Platform::ANDROID_GLES)
        {
            return std::shared_ptr<VertexBuffer>(new AndroidGLESVertexBuffer(data));
        }
        else
        {
            BR_ASSERT(false, "%s", "Can not create VertexBuffer. Unknown platform.");
            return nullptr;
        }
    }

    std::shared_ptr<IndexBuffer> Renderer::createIndexBuffer(const std::vector<uint32_t>& indices)
    {
        if(GameLoop::getPlatform() == Platform::ANDROID_GLES)
        {
            return std::shared_ptr<IndexBuffer>(new AndroidGLESIndexBuffer(indices));
        }
        else
        {
            BR_ASSERT(false, "%s", "Can not create IndexBuffer. Unknown platform.");
            return nullptr;
        }
    }

    std::unique_ptr<VertexArray> Renderer::createVertexArray()
    {
        if(GameLoop::getPlatform() == Platform::ANDROID_GLES)
        {
            return std::unique_ptr<VertexArray>(new AndroidGLESVertexArray());
        }
        else
        {
            BR_ASSERT(false, "%s", "Can not create VertexArray. Unknown platform.");
            return nullptr;
        }
    }

    std::shared_ptr<Shader> Renderer::createShader(const char* vertexPath, const char* fragmentPath)
    {
        if(GameLoop::getPlatform() == Platform::ANDROID_GLES)
        {
            return std::shared_ptr<Shader>(new AndroidGLESShader(vertexPath, fragmentPath));
        }
        else
        {
            BR_ASSERT(false, "%s", "Can not create Shader. Unknown platform.");
            return nullptr;
        }
    }

    std::unique_ptr<Texture> Renderer::createTexture(const char* path, TextureType type)
    {
        if(GameLoop::getPlatform() == Platform::ANDROID_GLES)
        {
            return std::unique_ptr<Texture>(new AndroidGLESTexture(path, type));
        }
        else
        {
            BR_ASSERT(false, "%s", "Can not create Texture. Unknown platform.");
            return nullptr;
        }
    }
}