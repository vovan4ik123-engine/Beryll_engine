#include "AndroidGLESShadowMapTexture.h"
#include "beryll/core/Window.h"
#include "beryll/core/BeryllConstants.h"
#include "beryll/platform/androidGLES/AndroidGLESGlobal.h"
#include "beryll/renderer/Renderer.h"
#include "beryll/gameObjects/BaseSimpleObject.h"
#include "beryll/gameObjects/BaseAnimatedObject.h"

#include <GLES3/gl32.h>
#include <GLES3/gl3ext.h>

namespace Beryll
{
    AndroidGLESShadowMapTexture::AndroidGLESShadowMapTexture(int width, int height) : m_mapWidth(width), m_mapHeight(height)
    {
        // Generate texture to store Z values of framebuffer.
        glGenTextures(1, &m_openGLID);
        glBindTexture(GL_TEXTURE_2D ,m_openGLID);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, m_mapWidth, m_mapHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // GL_NEAREST GL_LINEAR
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        const GLfloat borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        glBindTexture(GL_TEXTURE_2D, 0);

        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_defaultFBO);

        glGenFramebuffers(1, &m_depthMapFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);

        const GLenum noBuffers[] = {GL_NONE};
        glDrawBuffers(1, noBuffers);

        // Attach texture as framebuffer depth buffer.
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_openGLID, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D ,m_openGLID);
        GLESStateVariables::currentDiffuseTextureMat1ID0 = m_openGLID;
        glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);

        m_shaderSimple = Beryll::Renderer::createShader(BeryllConstants::simpleObjShadowMapVertexPath.data(),
                                                        BeryllConstants::simpleObjShadowMapFragmentPath.data());
        m_shaderAnimated = Beryll::Renderer::createShader(BeryllConstants::animatedObjShadowMapVertexPath.data(),
                                                          BeryllConstants::animatedObjShadowMapFragmentPath.data());

        // Bind. Can be bound 1 time to reserved GL_TEXTURE8. No need call bind() in every frame.
        glActiveTexture(GL_TEXTURE8);
        glBindTexture(GL_TEXTURE_2D, m_openGLID);
        GLESStateVariables::currentShadowMapTextureID8 = m_openGLID;
    }

    AndroidGLESShadowMapTexture::~AndroidGLESShadowMapTexture()
    {
        glActiveTexture(GL_TEXTURE8);
        glBindTexture(GL_TEXTURE_2D, 0);
        GLESStateVariables::currentShadowMapTextureID8 = 0;
        glDeleteTextures(1, &m_openGLID);

        glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);
        glDeleteFramebuffers(1, &m_depthMapFBO);
    }

    void AndroidGLESShadowMapTexture::bind()
    {
        if(GLESStateVariables::currentShadowMapTextureID8 != m_openGLID)
        {
            glActiveTexture(GL_TEXTURE8);
            glBindTexture(GL_TEXTURE_2D, m_openGLID);
            GLESStateVariables::currentShadowMapTextureID8 = m_openGLID;
        }
    }

    void AndroidGLESShadowMapTexture::unBind()
    {
        // This object can unbind only his own texture.
        if(GLESStateVariables::currentShadowMapTextureID8 == m_openGLID)
        {
            glActiveTexture(GL_TEXTURE8);
            glBindTexture(GL_TEXTURE_2D, 0);
            GLESStateVariables::currentShadowMapTextureID8 = 0;
        }
    }

    void AndroidGLESShadowMapTexture::drawIntoShadowMap(const std::vector<std::shared_ptr<Beryll::BaseSimpleObject>>& simpleObj,
                                                        const std::vector<std::shared_ptr<Beryll::BaseAnimatedObject>>& animatedObj,
                                                        const glm::mat4& VPLightMatrix)
    {
        glViewport(0, 0, m_mapWidth, m_mapHeight); // For texture resolution.
        glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glColorMask (GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        GLboolean polygonOffsetWasEnabled = glIsEnabled(GL_POLYGON_OFFSET_FILL);
        if(!polygonOffsetWasEnabled)
        {
            glEnable (GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(4.0f, 100.0f);
        }

        if(!simpleObj.empty())
        {
            m_shaderSimple->bind();
            for(const std::shared_ptr<Beryll::BaseSimpleObject>& so: simpleObj)
            {
                if(so->getIsEnabledDraw())
                {
                    m_shaderSimple->setMatrix4x4Float("MVPMatrix", VPLightMatrix * so->getModelMatrix());
                    so->useInternalShader = false;
                    so->useInternalMaterials = false;
                    so->draw();
                    so->useInternalShader = true;
                    so->useInternalMaterials = true;
                }
            }
        }

        if(!animatedObj.empty())
        {
            m_shaderAnimated->bind();
            std::string boneMatrixNameInShader;
            for(const std::shared_ptr<Beryll::BaseAnimatedObject>& ao: animatedObj)
            {
                if(ao->getIsEnabledDraw())
                {
                    m_shaderAnimated->setMatrix4x4Float("MVPMatrix", VPLightMatrix * ao->getModelMatrix());

                    uint32_t boneCount = ao->getBoneCount();
                    for(int i = 0; i < boneCount; ++i)
                    {
                        boneMatrixNameInShader = "bonesMatrices[";
                        boneMatrixNameInShader += std::to_string(i);
                        boneMatrixNameInShader += "]";
                        m_shaderAnimated->setMatrix4x4Float(boneMatrixNameInShader.c_str(), ao->getBoneMatrices()[i].finalWorldTransform);
                    }
                    ao->useInternalShader = false;
                    ao->useInternalMaterials = false;
                    ao->draw();
                    ao->useInternalShader = true;
                    ao->useInternalMaterials = true;
                }
            }
        }

        glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);
        glColorMask (GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        if(!polygonOffsetWasEnabled)
        {
            glDisable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(0.0f, 0.0f);
        }
        glViewport(0, 0, Window::getInstance()->getScreenWidth(), Window::getInstance()->getScreenHeight());
    }
}
