#include "AndroidGLESTexture.h"
#include "Beryll/Core/Log.h"

#include "LibsHeaders.h"

#include <GLES3/gl31.h>
#include <GLES3/gl3ext.h>

namespace Beryll
{
    AndroidGLESTexture::AndroidGLESTexture(const char* path, TextureType type)
    {
        m_type = type;

        const std::string strPath = path;

        auto result =  m_textures.find(strPath);
        if(result != m_textures.end())
        {
            // texture was added before, use it
            //BR_INFO("%s", "texture was added before");
            m_textureID = result->second;
            return;
        }

        BR_ASSERT((strPath.find_last_of('.') != std::string::npos), "Texture does not have extension:%s", strPath.c_str());

        BR_ASSERT(((strPath.substr(strPath.find_last_of('.')) == ".png") ||
                   (strPath.substr(strPath.find_last_of('.')) == ".jpg")),
                  "Supported only .png or .jpg textures:%s", strPath.c_str());

        BR_ASSERT(((type == TextureType::DIFFUSE_TEXTURE) || (type == TextureType::SPECULAR_TEXTURE)), "%s", "Not correct texture type");

        SDL_RWops* rw = SDL_RWFromFile(strPath.c_str(), "rb");
        BR_ASSERT((rw != nullptr), "Load texture failed:%s", strPath.c_str());

        SDL_Surface* surface = IMG_Load_RW(rw, 1);
        BR_ASSERT((surface != nullptr), "Create surface failed:%s", strPath.c_str());

        glGenTextures(1, &m_textureID);
        glBindTexture(GL_TEXTURE_2D, m_textureID);

        int pixelFormat = GL_RGB;
        if(4 == surface->format->BytesPerPixel) pixelFormat = GL_RGBA;

        BR_ASSERT((surface->format->BytesPerPixel == 3 || surface->format->BytesPerPixel == 4), "Load texture failed:%s. Use 24 or 32 bit depth", strPath.c_str());

        glTexImage2D(GL_TEXTURE_2D, 0, pixelFormat, surface->w, surface->h, 0, pixelFormat, GL_UNSIGNED_BYTE, surface->pixels);

        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindTexture(GL_TEXTURE_2D, 0);

        m_textures.insert(std::make_pair(strPath, m_textureID)); // add to map
    }

    AndroidGLESTexture::~AndroidGLESTexture()
    {
        // keep textures in static map
    }

    std::map<std::string, uint32_t> AndroidGLESTexture::m_textures;

    void AndroidGLESTexture::bind()
    {
        if(m_type == TextureType::DIFFUSE_TEXTURE)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_textureID);
        }
        else if(m_type == TextureType::SPECULAR_TEXTURE)
        {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, m_textureID);
        }
    }

    void AndroidGLESTexture::unBind()
    {
        if(m_type == TextureType::DIFFUSE_TEXTURE)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        else if(m_type == TextureType::SPECULAR_TEXTURE)
        {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }
}