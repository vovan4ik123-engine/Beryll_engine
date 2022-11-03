#include "AndroidGLESShadowMapTexture.h"
#include "Beryll/Core/Log.h"
#include "Beryll/Core/Window.h"
#include "Beryll/Platform/AndroidGLES/AndroidGLESGlobal.h"

#include <GLES3/gl32.h>
#include <GLES3/gl3ext.h>

namespace Beryll
{
    AndroidGLESShadowMapTexture::AndroidGLESShadowMapTexture(int width, int height) : m_mapWidth(width), m_mapHeight(height)
    {
        m_type = TextureType::SHADOW_MAP_TEXTURE;

        // Generate texture to store Z values of framebuffer
        glGenTextures(1, &m_openGLID);
        glBindTexture(GL_TEXTURE_2D ,m_openGLID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, m_mapWidth, m_mapHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
        float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        glBindTexture(GL_TEXTURE_2D ,0);

        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_defaultFBO);

        glGenFramebuffers(1, &m_depthMapFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
        glDrawBuffers(1, GL_NONE);

        // Attach texture as framebuffer depth buffer
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_openGLID, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D ,m_openGLID);
        GLESStateVariables::currentTexture0 = m_openGLID;

        glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);
    }

    AndroidGLESShadowMapTexture::~AndroidGLESShadowMapTexture()
    {
        glDeleteTextures(GL_TEXTURE_2D, &m_openGLID);

        glDeleteFramebuffers(1, &m_depthMapFBO);
    }

    void AndroidGLESShadowMapTexture::bind()
    {
        if(GLESStateVariables::currentTexture11 != m_openGLID)
        {
            glActiveTexture(GL_TEXTURE11);
            glBindTexture(GL_TEXTURE_2D, m_openGLID);
            GLESStateVariables::currentTexture11 = m_openGLID;
        }
    }

    void AndroidGLESShadowMapTexture::unBind()
    {
        // this object can unbind only his own texture
        if(GLESStateVariables::currentTexture11 == m_openGLID)
        {
            glActiveTexture(GL_TEXTURE11);
            glBindTexture(GL_TEXTURE_2D, 0);
            GLESStateVariables::currentTexture11 = 0;
        }
    }

    void AndroidGLESShadowMapTexture::drawIntoShadowMap(const std::vector<std::shared_ptr<Beryll::BaseSimpleObject>>& simpleObj,
                                                        const std::vector<std::shared_ptr<Beryll::BaseAnimatedObject>>& animatedObj,
                                                        const std::shared_ptr<Beryll::Shader>& shaderSimple,
                                                        const std::shared_ptr<Beryll::Shader>& shaderAnimated,
                                                        const glm::mat4& VPMatrix)
    {
        glViewport(0, 0, m_mapWidth, m_mapHeight); // for texture resolution
        glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glColorMask (GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        GLboolean polygonOffsetWasEnabled = glIsEnabled(GL_POLYGON_OFFSET_FILL);
        if(!polygonOffsetWasEnabled)
        {
            glEnable (GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(4.0f, 100.0f);
        }
        glCullFace(GL_FRONT);

        shaderSimple->bind();

        for(const std::shared_ptr<Beryll::BaseSimpleObject>& so: simpleObj)
        {
            if(so->getIsEnabledOnScene())
            {
                shaderSimple->setMatrix4x4Float("MVPMatrix", VPMatrix * so->getModelMatrix());
                so->useInternalShader = false;
                so->useInternalTextures = false;
                so->draw();
            }
        }

        std::string boneMatrixNameInShader;
        shaderAnimated->bind();
        for(const std::shared_ptr<Beryll::BaseAnimatedObject>& ao: animatedObj)
        {
            if(ao->getIsEnabledOnScene())
            {
                shaderAnimated->setMatrix4x4Float("MVPMatrix", VPMatrix * ao->getModelMatrix());

                uint32_t boneCount = ao->getBoneCount();
                for(int i = 0; i < boneCount; ++i)
                {
                    boneMatrixNameInShader = "bonesMatrices[";
                    boneMatrixNameInShader += std::to_string(i);
                    boneMatrixNameInShader += "]";
                    shaderAnimated->setMatrix4x4Float(boneMatrixNameInShader.c_str(), ao->getBoneMatrices()[i].finalWorldTransform);
                }
                ao->useInternalShader = false;
                ao->useInternalTextures = false;
                ao->draw();
            }
        }

        glCullFace(GL_BACK);
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