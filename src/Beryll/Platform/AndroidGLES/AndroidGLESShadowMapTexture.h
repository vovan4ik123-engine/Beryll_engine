#pragma once

#include "LibsHeaders.h"
#include "CppHeaders.h"

#include "Beryll/Renderer/Texture.h"
#include "Beryll/Renderer/Shader.h"
#include "Beryll/GameObjects/BaseSimpleObject.h"
#include "Beryll/GameObjects/BaseAnimatedObject.h"

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

    private:
        friend class Renderer;
        /*
         * simpleObj - simple objects which wants shadows
         * animatedObj - animated objects which wants shadows
         * shaderSimple - shader for simple objects for generate shadow map
         * shaderAnimated - shader for animated objects for generate shadow map
         * VP_matrix - projection matrix from light point of view
         */
        AndroidGLESShadowMapTexture(const std::vector<std::shared_ptr<Beryll::BaseSimpleObject>>& simpleObj,
                                    const std::vector<std::shared_ptr<Beryll::BaseAnimatedObject>>& animatedObj,
                                    const std::shared_ptr<Beryll::Shader>& shaderSimple,
                                    const std::shared_ptr<Beryll::Shader>& shaderAnimated,
                                    const glm::mat4& VP_matrix);

        uint32_t m_depthMapFBO = 0;
        uint32_t m_openGLID = 0;

        static uint32_t m_currentShadowMapTextureID; // should contain currently bound shadow map texture id
                                                     // prevent bind same texture many times
    };
}
