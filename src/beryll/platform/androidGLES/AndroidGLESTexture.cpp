#include "AndroidGLESTexture.h"
#include "beryll/core/Log.h"
#include "beryll/platform/androidGLES/AndroidGLESGlobal.h"

#include <GLES3/gl32.h>
#include <GLES3/gl3ext.h>

namespace Beryll
{
    std::map<const std::string, std::shared_ptr<uint32_t>> AndroidGLESTexture::m_textures;

    AndroidGLESTexture::AndroidGLESTexture(const char* path, TextureType type) : m_ID(path)
    {
        m_type = type;

        auto result =  m_textures.find(m_ID);
        if(result != m_textures.end())
        {
            // Texture was created before, use it.
            //BR_INFO("%s", "Texture was created before.");
            m_openGLID = result->second; // Copy shared pointer.
            return;
        }

        BR_ASSERT((m_ID.find_last_of('.') != std::string::npos), "Texture does not have extension: %s", m_ID.c_str());

        BR_ASSERT(((m_ID.substr(m_ID.find_last_of('.')) == ".png") ||
                   (m_ID.substr(m_ID.find_last_of('.')) == ".jpg")),
                  "Supported only .png or .jpg textures: %s", m_ID.c_str());

        BR_ASSERT((type == TextureType::DIFFUSE_TEXTURE_MAT_1 || type == TextureType::SPECULAR_TEXTURE_MAT_1 || type == TextureType::NORMAL_MAP_TEXTURE_MAT_1 ||
                   type == TextureType::DIFFUSE_TEXTURE_MAT_2 || type == TextureType::SPECULAR_TEXTURE_MAT_2 || type == TextureType::NORMAL_MAP_TEXTURE_MAT_2 ||
                   type == TextureType::BLEND_TEXTURE_MAT_2), "%s", "Wrong texture type");

        SDL_IOStream* rw = SDL_IOFromFile(m_ID.c_str(), "rb");
        BR_ASSERT((rw != nullptr), "Load texture failed: %s", m_ID.c_str());

        SDL_Surface* surface = IMG_Load_IO(rw, true);
        BR_ASSERT((surface != nullptr), "Create surface failed: %s", m_ID.c_str());

        BR_ASSERT((SDL_BYTESPERPIXEL(surface->format) > 0 && SDL_BYTESPERPIXEL(surface->format) <= 4), "Load texture failed: %s. Depth = 0 or > 4", m_ID.c_str());

        int pixelFormat = GL_RGB;
        if(SDL_BYTESPERPIXEL(surface->format) == 4) pixelFormat = GL_RGBA;
        else if(SDL_BYTESPERPIXEL(surface->format) == 1) pixelFormat = GL_RED;

        m_openGLID = std::make_shared<uint32_t>();
        m_width = surface->w;
        m_height = surface->h;

        glGenTextures(1, m_openGLID.get());
        glBindTexture(GL_TEXTURE_2D, *m_openGLID);

        glTexImage2D(GL_TEXTURE_2D, 0, pixelFormat, surface->w, surface->h, 0, pixelFormat, GL_UNSIGNED_BYTE, surface->pixels);

        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindTexture(GL_TEXTURE_2D, 0);

        SDL_DestroySurface(surface);
        m_textures.insert(std::make_pair(m_ID, m_openGLID)); // Add to map.
        //BR_INFO("%s", "Texture created.");
    }

    AndroidGLESTexture::~AndroidGLESTexture()
    {
        if(m_openGLID.use_count() <= 2)
        {
            // use_count() <= 2 means only shared_ptr in this class left and in map.
            // If we destroy this m_openGLID (last copy except copy in map) also delete from OpenGL and map.
            auto result =  m_textures.find(m_ID);
            if(result != m_textures.end())
            {
                unBindNotVirtual();

                glDeleteTextures(1, m_openGLID.get());
                m_textures.erase(result);
                //BR_INFO("%s", "Texture destroyed.");
            }
        }
    }

    void AndroidGLESTexture::bind()
    {
        // Dont bind if m_openGLID already bound.
        if(m_type == TextureType::DIFFUSE_TEXTURE_MAT_1) // DONT combine two if() here. Most objects will have one diffuse texture
        {                                                // and if/else chain should stop checks in first if().
            if(GLESStateVariables::currentDiffuseTextureMat1ID0 != *m_openGLID)
            {
                //BR_INFO("%s", "DIFFUSE_TEXTURE_MAT_1");
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, *m_openGLID);
                GLESStateVariables::currentDiffuseTextureMat1ID0 = *m_openGLID;
            }
        }
        else if(m_type == TextureType::SPECULAR_TEXTURE_MAT_1)
        {
            if(GLESStateVariables::currentSpecularTextureMat1ID1 != *m_openGLID)
            {
                //BR_INFO("%s", "SPECULAR_TEXTURE_MAT_1");
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, *m_openGLID);
                GLESStateVariables::currentSpecularTextureMat1ID1 = *m_openGLID;
            }
        }
        else if(m_type == TextureType::NORMAL_MAP_TEXTURE_MAT_1)
        {
            if(GLESStateVariables::currentNormalMapTextureMat1ID2 != *m_openGLID)
            {
                //BR_INFO("%s", "NORMAL_MAP_TEXTURE_MAT_1");
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, *m_openGLID);
                GLESStateVariables::currentNormalMapTextureMat1ID2 = *m_openGLID;
            }
        }
        else if(m_type == TextureType::DIFFUSE_TEXTURE_MAT_2)
        {
            if(GLESStateVariables::currentDiffuseTextureMat2ID3 != *m_openGLID)
            {
                //BR_INFO("%s", "bind DIFFUSE_TEXTURE_MAT_2");
                glActiveTexture(GL_TEXTURE3);
                glBindTexture(GL_TEXTURE_2D, *m_openGLID);
                GLESStateVariables::currentDiffuseTextureMat2ID3 = *m_openGLID;
            }
        }
        else if(m_type == TextureType::SPECULAR_TEXTURE_MAT_2)
        {
            if(GLESStateVariables::currentSpecularTextureMat2ID4 != *m_openGLID)
            {
                //BR_INFO("%s", "bind SPECULAR_TEXTURE_MAT_2");
                glActiveTexture(GL_TEXTURE4);
                glBindTexture(GL_TEXTURE_2D, *m_openGLID);
                GLESStateVariables::currentSpecularTextureMat2ID4 = *m_openGLID;
            }
        }
        else if(m_type == TextureType::NORMAL_MAP_TEXTURE_MAT_2)
        {
            if(GLESStateVariables::currentNormalMapTextureMat2ID5 != *m_openGLID)
            {
                //BR_INFO("%s", "bind NORMAL_MAP_TEXTURE_MAT_2");
                glActiveTexture(GL_TEXTURE5);
                glBindTexture(GL_TEXTURE_2D, *m_openGLID);
                GLESStateVariables::currentNormalMapTextureMat2ID5 = *m_openGLID;
            }
        }
        else if(m_type == TextureType::BLEND_TEXTURE_MAT_2)
        {
            if(GLESStateVariables::currentBlendTextureMat2ID6 != *m_openGLID)
            {
                //BR_INFO("%s", "bind BLEND_TEXTURE_MAT_2");
                glActiveTexture(GL_TEXTURE6);
                glBindTexture(GL_TEXTURE_2D, *m_openGLID);
                GLESStateVariables::currentBlendTextureMat2ID6 = *m_openGLID;
            }
        }
    }

    void AndroidGLESTexture::unBind()
    {
        unBindNotVirtual();
    }

    void AndroidGLESTexture::unBindNotVirtual()
    {
        // This object can unbind only his own texture.
        if(m_type == TextureType::DIFFUSE_TEXTURE_MAT_1)
        {
            if(GLESStateVariables::currentDiffuseTextureMat1ID0 == *m_openGLID)
            {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, 0);
                GLESStateVariables::currentDiffuseTextureMat1ID0 = 0;
            }
        }
        else if(m_type == TextureType::SPECULAR_TEXTURE_MAT_1)
        {
            if(GLESStateVariables::currentSpecularTextureMat1ID1 == *m_openGLID)
            {
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, 0);
                GLESStateVariables::currentSpecularTextureMat1ID1 = 0;
            }
        }
        else if(m_type == TextureType::NORMAL_MAP_TEXTURE_MAT_1)
        {
            if(GLESStateVariables::currentNormalMapTextureMat1ID2 == *m_openGLID)
            {
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, 0);
                GLESStateVariables::currentNormalMapTextureMat1ID2 = 0;
            }
        }
        else if(m_type == TextureType::DIFFUSE_TEXTURE_MAT_2)
        {
            if(GLESStateVariables::currentDiffuseTextureMat2ID3 == *m_openGLID)
            {
                glActiveTexture(GL_TEXTURE3);
                glBindTexture(GL_TEXTURE_2D, 0);
                GLESStateVariables::currentDiffuseTextureMat2ID3 = 0;
            }
        }
        else if(m_type == TextureType::SPECULAR_TEXTURE_MAT_2)
        {
            if(GLESStateVariables::currentSpecularTextureMat2ID4 == *m_openGLID)
            {
                glActiveTexture(GL_TEXTURE4);
                glBindTexture(GL_TEXTURE_2D, 0);
                GLESStateVariables::currentSpecularTextureMat2ID4 = 0;
            }
        }
        else if(m_type == TextureType::NORMAL_MAP_TEXTURE_MAT_2)
        {
            if(GLESStateVariables::currentNormalMapTextureMat2ID5 == *m_openGLID)
            {
                glActiveTexture(GL_TEXTURE5);
                glBindTexture(GL_TEXTURE_2D, 0);
                GLESStateVariables::currentNormalMapTextureMat2ID5 = 0;
            }
        }
        else if(m_type == TextureType::BLEND_TEXTURE_MAT_2)
        {
            if(GLESStateVariables::currentBlendTextureMat2ID6 == *m_openGLID)
            {
                glActiveTexture(GL_TEXTURE6);
                glBindTexture(GL_TEXTURE_2D, 0);
                GLESStateVariables::currentBlendTextureMat2ID6 = 0;
            }
        }
    }
}
