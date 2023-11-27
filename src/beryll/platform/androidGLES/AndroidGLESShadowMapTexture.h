#pragma once

#include "LibsHeaders.h"
#include "CppHeaders.h"

#include "beryll/renderer/Texture.h"
#include "beryll/renderer/Shader.h"
#include "beryll/gameObjects/BaseSimpleObject.h"
#include "beryll/gameObjects/BaseAnimatedObject.h"

namespace Beryll
{
    // That is OpenGL representation of shadow map texture created from drawing scene from light perspective to FrameBuffer
    class AndroidGLESShadowMapTexture : public Texture
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
        /*
         * simpleObj - simple objects which wants shadows
         * animatedObj - animated objects which wants shadows
         * shaderSimple - shader for simple objects for generate shadow map
         * shaderAnimated - shader for animated objects for generate shadow map
         * VPMatrix - projection matrix from light point of view
         */
        AndroidGLESShadowMapTexture(int width, int height);

        int m_defaultFBO = 0;
        uint32_t m_depthMapFBO = 0;
        uint32_t m_openGLID = 0;

        std::shared_ptr<Beryll::Shader> m_shaderSimple;
        std::shared_ptr<Beryll::Shader> m_shaderAnimated;

        const int m_mapWidth;
        const int m_mapHeight;
    };
}
