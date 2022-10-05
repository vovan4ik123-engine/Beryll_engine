#include "AndroidGLESTexture.h"
#include "Beryll/Core/Log.h"

#include <GLES3/gl31.h>
#include <GLES3/gl3ext.h>

namespace Beryll
{
    AndroidGLESTexture::AndroidGLESTexture(const char* path, TextureType type) : m_ID(path)
    {
        m_type = type;

        auto result =  m_textures.find(m_ID);
        if(result != m_textures.end())
        {
            // texture was added before, use it
            //BR_INFO("%s", "texture was added before");
            m_openGLID = result->second; // copy shared pointer
            return;
        }

        BR_ASSERT((m_ID.find_last_of('.') != std::string::npos), "Texture does not have extension:%s", m_ID.c_str());

        BR_ASSERT(((m_ID.substr(m_ID.find_last_of('.')) == ".png") ||
                   (m_ID.substr(m_ID.find_last_of('.')) == ".jpg")),
                  "Supported only .png or .jpg textures:%s", m_ID.c_str());

        BR_ASSERT(((type == TextureType::DIFFUSE_TEXTURE) || (type == TextureType::SPECULAR_TEXTURE)), "%s", "Not correct texture type");

        SDL_RWops* rw = SDL_RWFromFile(m_ID.c_str(), "rb");
        BR_ASSERT((rw != nullptr), "Load texture failed:%s", m_ID.c_str());

        SDL_Surface* surface = IMG_Load_RW(rw, 1);
        BR_ASSERT((surface != nullptr), "Create surface failed:%s", m_ID.c_str());

        BR_ASSERT((surface->format->BytesPerPixel == 3 || surface->format->BytesPerPixel == 4), "Load texture failed:%s. Use 24 or 32 bit depth", m_ID.c_str());

        int pixelFormat = GL_RGB;
        if(4 == surface->format->BytesPerPixel) pixelFormat = GL_RGBA;

        m_openGLID = std::make_shared<uint32_t>();

        glGenTextures(1, m_openGLID.get());
        glBindTexture(GL_TEXTURE_2D, *m_openGLID);

        glTexImage2D(GL_TEXTURE_2D, 0, pixelFormat, surface->w, surface->h, 0, pixelFormat, GL_UNSIGNED_BYTE, surface->pixels);

        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindTexture(GL_TEXTURE_2D, 0);

        SDL_FreeSurface(surface);
        m_textures.insert(std::make_pair(m_ID, m_openGLID)); // add to map
    }

    AndroidGLESTexture::~AndroidGLESTexture()
    {
        if(m_openGLID.use_count() <= 2)
        {
            // use_count() <= 2 means only shared_ptr in this class left and in map
            // if we destroy this m_openGLID (last copy except copy in map) also delete from OpenGL and map.
            auto result =  m_textures.find(m_ID);
            if(result != m_textures.end())
            {
                glDeleteTextures(1, m_openGLID.get());
                m_textures.erase(result);
            }
        }
    }

    std::map<const std::string, std::shared_ptr<uint32_t>> AndroidGLESTexture::m_textures;

    void AndroidGLESTexture::bind()
    {
        if(m_type == TextureType::DIFFUSE_TEXTURE)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, *m_openGLID);
        }
        else if(m_type == TextureType::SPECULAR_TEXTURE)
        {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, *m_openGLID);
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