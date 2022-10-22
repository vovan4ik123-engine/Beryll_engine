#pragma once

#include "LibsHeaders.h"
#include "CppHeaders.h"

#include "Beryll/Renderer/Texture.h"

namespace Beryll
{
    // This texture is loaded from file .png or .jpg
    class AndroidGLESTexture : public Texture
    {
    public:
        AndroidGLESTexture() = delete;
        ~AndroidGLESTexture() override;

        void bind() override;
        void unBind() override;

        void drawIntoShadowMap(const std::vector<std::shared_ptr<Beryll::BaseSimpleObject>>& simpleObj,
                               const std::vector<std::shared_ptr<Beryll::BaseAnimatedObject>>& animatedObj,
                               const std::shared_ptr<Beryll::Shader>& shaderSimple,
                               const std::shared_ptr<Beryll::Shader>& shaderAnimated,
                               const glm::mat4& VPMatrix) override
        {
            BR_ASSERT(false, "%s", "drawIntoShadowMap() can be called only from shadow map texture");
        }

    private:
        friend class Renderer;
        /*
         * path - path to file in asset folder
         * type
         */
        AndroidGLESTexture(const char* path, TextureType type);

        // keep textures in static map in subclasses for reuse them
        // key = m_ID
        // value = m_openGLID
        static std::map<const std::string, std::shared_ptr<uint32_t>> m_textures;

        const std::string m_ID; // ID in static map = texture path
                                // if many objects load same texture, texture ID will same for all of them

        std::shared_ptr<uint32_t> m_openGLID; // ID in OpenGL
                                              // will copied across all objects with same m_ID
    };
}
