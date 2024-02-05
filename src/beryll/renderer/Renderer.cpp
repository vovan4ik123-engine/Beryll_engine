#include "Renderer.h"
#include "beryll/core/Log.h"
#include "beryll/renderer/Camera.h"

#if defined(ANDROID)
    #include <GLES3/gl32.h>
    #include <GLES3/gl3ext.h>

    #include "beryll/platform/androidGLES/AndroidGLESBuffer.h"
    #include "beryll/platform/androidGLES/AndroidGLESVertexArray.h"
    #include "beryll/platform/androidGLES/AndroidGLESShader.h"
    #include "beryll/platform/androidGLES/AndroidGLESTexture.h"
    #include "beryll/platform/androidGLES/AndroidGLESShadowMapTexture.h"
    #include "beryll/platform/androidGLES/AndroidGLESSkyBox.h"

    #include "beryll/platform/androidGLES/AndroidGLESGlobal.h"
#elif defined(APPLE)

#endif

namespace Beryll
{
    std::shared_ptr<VertexBuffer> Renderer::createStaticVertexBuffer(const std::vector<glm::vec2>& data)
    {
#if defined(ANDROID)
        return std::shared_ptr<VertexBuffer>(new AndroidGLESStaticVertexBuffer(data));
#elif defined(APPLE)

#else
        BR_ASSERT(false, "%s", "Can not create VertexBuffer. Unknown platform.");
        return nullptr;
#endif
    }

    std::shared_ptr<VertexBuffer> Renderer::createStaticVertexBuffer(const std::vector<glm::vec3>& data)
    {
#if defined(ANDROID)
        return std::shared_ptr<VertexBuffer>(new AndroidGLESStaticVertexBuffer(data));
#elif defined(APPLE)

#else
        BR_ASSERT(false, "%s", "Can not create VertexBuffer. Unknown platform.");
        return nullptr;
#endif
    }

    std::shared_ptr<VertexBuffer> Renderer::createStaticVertexBuffer(const std::vector<glm::vec4>& data)
    {
#if defined(ANDROID)
        return std::shared_ptr<VertexBuffer>(new AndroidGLESStaticVertexBuffer(data));
#elif defined(APPLE)

#else
        BR_ASSERT(false, "%s", "Can not create VertexBuffer. Unknown platform.");
        return nullptr;
#endif
    }

    std::shared_ptr<VertexBuffer> Renderer::createStaticVertexBuffer(const std::vector<glm::ivec4>& data)
    {
#if defined(ANDROID)
        return std::shared_ptr<VertexBuffer>(new AndroidGLESStaticVertexBuffer(data));
#elif defined(APPLE)

#else
        BR_ASSERT(false, "%s", "Can not create VertexBuffer. Unknown platform.");
        return nullptr;
#endif
    }

    std::shared_ptr<VertexBuffer> Renderer::createStaticVertexBuffer(const std::vector<glm::mat4>& data)
    {
#if defined(ANDROID)
        return std::shared_ptr<VertexBuffer>(new AndroidGLESStaticVertexBuffer(data));
#elif defined(APPLE)

#else
        BR_ASSERT(false, "%s", "Can not create VertexBuffer. Unknown platform.");
        return nullptr;
#endif
    }

    std::shared_ptr<VertexBuffer> Renderer::createDynamicVertexBuffer(VertexAttribType type, VertexAttribSize size, uint32_t maxSizeBytes)
    {
#if defined(ANDROID)
        return std::shared_ptr<VertexBuffer>(new AndroidGLESDynamicVertexBuffer(type, size, maxSizeBytes));
#elif defined(APPLE)

#else
        BR_ASSERT(false, "%s", "Can not create VertexBuffer. Unknown platform.");
        return nullptr;
#endif
    }

    std::shared_ptr<IndexBuffer> Renderer::createStaticIndexBuffer(const std::vector<uint32_t>& indices)
    {
#if defined(ANDROID)
        return std::shared_ptr<IndexBuffer>(new AndroidGLESStaticIndexBuffer(indices));
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

    std::unique_ptr<ShadowMap> Renderer::createShadowMap(int width, int height)
    {
#if defined(ANDROID)
        return std::unique_ptr<ShadowMap>(new AndroidGLESShadowMapTexture(width, height));
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

    void Renderer::drawObject(const std::shared_ptr<Beryll::BaseSimpleObject>& simpleObj,
                              const glm::mat4& modelMatrix,
                              const std::shared_ptr<Shader>& shader)
    {
        if(shader)
        {
            shader->bind();
            shader->setMatrix4x4Float("MVPMatrix", Beryll::Camera::getViewProjection() * modelMatrix);

            simpleObj->useInternalShader = false;
            simpleObj->draw();
        }
        else
        {
            simpleObj->useInternalShader = true;
            simpleObj->draw();
        }
    }

    void Renderer::drawObject(const std::shared_ptr<Beryll::BaseAnimatedObject>& animObj,
                              const glm::mat4& modelMatrix,
                              const std::shared_ptr<Shader>& shader)
    {
        if(shader)
        {
            static std::string boneMatrixNameInShader;
            static uint32_t boneCount = 0;

            shader->bind();
            shader->setMatrix4x4Float("MVPMatrix", Beryll::Camera::getViewProjection() * modelMatrix);

            boneCount = animObj->getBoneCount();
            for(int i = 0; i < boneCount; ++i)
            {
                boneMatrixNameInShader = "bonesMatrices[";
                boneMatrixNameInShader += std::to_string(i);
                boneMatrixNameInShader += "]";
                shader->setMatrix4x4Float(boneMatrixNameInShader.c_str(), animObj->getBoneMatrices()[i].finalWorldTransform);
            }

            animObj->useInternalShader = false;
            animObj->draw();
        }
        else
        {
            animObj->useInternalShader = true;
            animObj->draw();
        }
    }

    void Renderer::enableFaceCulling()
    {
#if defined(ANDROID)
        if(!GLESStateVariables::faceCullingEnabled)
        {
            glEnable(GL_CULL_FACE);
            GLESStateVariables::faceCullingEnabled = true;
        }
#elif defined(APPLE)

#endif
    }

    void Renderer::disableFaceCulling()
    {
#if defined(ANDROID)
        if(GLESStateVariables::faceCullingEnabled)
        {
            glDisable(GL_CULL_FACE);
            GLESStateVariables::faceCullingEnabled = false;
        }
#elif defined(APPLE)

#endif
    }
}
