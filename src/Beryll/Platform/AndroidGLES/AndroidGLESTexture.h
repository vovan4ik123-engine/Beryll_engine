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
         * numberInShader - index number of sampler2D ... for texture in fragment shader
         *                  (use first sampler2D for diffuse texture with indexInShader = 0)
         *                  (use second sampler2D for specular texture with indexInShader = 1)
         */
        AndroidGLESTexture(const char* path, uint32_t indexInShader = -1);

        // keep textures in static map in subclasses for reuse them
        // key = texturePath
        // value = textureID
        static std::map<std::string, uint32_t> m_textures;

        uint32_t m_textureID = 0;
        uint32_t m_indexInShader = 0;
    };
}
