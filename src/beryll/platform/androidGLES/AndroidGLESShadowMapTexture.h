#pragma once

#include "beryll/renderer/Shader.h"
#include "beryll/renderer/ShadowMap.h"

namespace Beryll
{
    // That is OpenGL representation of shadow map texture created from drawing scene from light perspective to FrameBuffer.
    class AndroidGLESShadowMapTexture : public ShadowMap
    {
    public:
        AndroidGLESShadowMapTexture() = delete;
        ~AndroidGLESShadowMapTexture() override;

        void bind() override;
        void unBind() override;
        uint32_t getID() override { return m_openGLID; }
        int getWidth() override { return m_mapWidth; }
        int getHeight() override { return m_mapHeight; }

        void drawIntoShadowMap(const std::vector<std::shared_ptr<Beryll::BaseSimpleObject>>& simpleObj,
                               const std::vector<std::shared_ptr<Beryll::BaseAnimatedObject>>& animatedObj,
                               const glm::mat4& VPLightMatrix) override;

    private:
        friend class Renderer;
        AndroidGLESShadowMapTexture(int width, int height);

        int m_defaultFBO = 0;
        uint32_t m_depthMapFBO = 0;
        uint32_t m_openGLID = 0;

        std::shared_ptr<Beryll::Shader> m_shaderSimple; // Shader for simple objects to generate shadow map.
        std::shared_ptr<Beryll::Shader> m_shaderAnimated; // Shader for animated objects to generate shadow map.

        const int m_mapWidth;
        const int m_mapHeight;
    };
}
