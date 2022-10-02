#pragma once

#include "LibsHeaders.h"
#include "CppHeaders.h"

#include "Beryll/Renderer/Texture.h"

namespace Beryll
{
    class AndroidGLESTexture : public Texture
    {
    public:
        AndroidGLESTexture() = delete;
        ~AndroidGLESTexture() override;

        void bind() override;
        void unBind() override;

    private:
        friend class Renderer;
        /*
         * path - path to file in asset folder
         * type
         */
        AndroidGLESTexture(const char* path, TextureType type);

        // keep textures in static map in subclasses for reuse them
        // key = texturePath
        // value = textureID
        static std::map<std::string, uint32_t> m_textures;

        uint32_t m_textureID = 0;
        TextureType m_type = TextureType::UNKNOWN;
    };
}
