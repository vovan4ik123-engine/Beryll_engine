#include "AndroidGLESShadowMapTexture.h"
#include "Beryll/Core/Log.h"
#include "Beryll/Core/Window.h"

#include <GLES3/gl31.h>
#include <GLES3/gl3ext.h>

namespace Beryll
{
    uint32_t AndroidGLESShadowMapTexture::m_currentShadowMapTextureID = 0;

    AndroidGLESShadowMapTexture::AndroidGLESShadowMapTexture(const std::vector<std::shared_ptr<Beryll::BaseSimpleObject>>& simpleObj,
                                                             const std::vector<std::shared_ptr<Beryll::BaseAnimatedObject>>& animatedObj,
                                                             const std::shared_ptr<Beryll::Shader>& shaderSimple,
                                                             const std::shared_ptr<Beryll::Shader>& shaderAnimated,
                                                             const glm::mat4& VP_matrix)
    {
        m_type = TextureType::SHADOW_MAP_TEXTURE;

        if(m_openGLID != 0)
        {
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, 0);
            m_currentShadowMapTextureID = 0;

            glDeleteTextures(GL_TEXTURE_2D ,&m_openGLID);
            m_openGLID = 0;
        }

        if(m_depthMapFBO != 0)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glDeleteFramebuffers(1, &m_depthMapFBO); // TODO: HERE ?????
            m_depthMapFBO = 0;
        }

        // Generate texture to store Z values of framebuffer
        glGenTextures(1, &m_openGLID);
        glBindTexture(GL_TEXTURE_2D ,m_openGLID);

        const uint32_t MAP_WIDTH = 1024;
        const uint32_t MAP_HEIGHT = 1024;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, MAP_WIDTH, MAP_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glGenFramebuffers(1, &m_depthMapFBO); // TODO: OR REUSE ??????
        glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
        // Attach texture as framebuffer depth buffer
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_openGLID, 0);
        glDrawBuffers(GL_NONE, GL_NONE);
        glReadBuffer(GL_NONE);

        glViewport(0, 0, MAP_WIDTH, MAP_HEIGHT); // for texture resolution
        glClear(GL_DEPTH_BUFFER_BIT);

        // bind shaders
        // set uniforms
        // draw all scene objects

        shaderSimple->bind();
        //shaderSimple->setMatrix4x4Float("MVP_matrix", VP_matrix * model);
        for(const std::shared_ptr<Beryll::BaseSimpleObject>& : simpleObj)
        {

        }


        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, Window::getInstance()->getScreenWidth(), Window::getInstance()->getScreenHeight()); // return back
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    AndroidGLESShadowMapTexture::~AndroidGLESShadowMapTexture()
    {
        if(m_openGLID != 0)
        {
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, 0);
            m_currentShadowMapTextureID = 0;

            glDeleteTextures(GL_TEXTURE_2D ,&m_openGLID);
            m_openGLID = 0;
        }

        if(m_depthMapFBO != 0)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glDeleteFramebuffers(1, &m_depthMapFBO);
            m_depthMapFBO = 0;
        }
    }

    void AndroidGLESShadowMapTexture::bind()
    {
        if(m_currentShadowMapTextureID != m_openGLID)
        {
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, m_openGLID);
            m_currentShadowMapTextureID = m_openGLID;
        }
    }

    void AndroidGLESShadowMapTexture::unBind()
    {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, 0);
        m_currentShadowMapTextureID = 0;
    }
}