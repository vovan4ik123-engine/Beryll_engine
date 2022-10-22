#include "AndroidGLESSkyBox.h"
#include "Beryll/Core/Log.h"
#include "Beryll/Renderer/Camera.h"
#include "Beryll/Platform/AndroidGLES/AndroidGLESGlobal.h"

#include <GLES3/gl32.h>
#include <GLES3/gl3ext.h>

namespace Beryll
{
    AndroidGLESSkyBox::AndroidGLESSkyBox(std::string folderPath)
    {
        std::string rightSide = folderPath + "/right.jpg";
        std::string leftSide = folderPath + "/left.jpg";
        std::string topSide = folderPath + "/top.jpg";
        std::string bottomSide = folderPath + "/bottom.jpg";
        std::string backSide = folderPath + "/back.jpg";
        std::string frontSide = folderPath + "/front.jpg";

        SDL_RWops* rwRight = SDL_RWFromFile(rightSide.c_str(), "rb");
        BR_ASSERT((rwRight != nullptr), "Load cube texture failed:%s", rightSide.c_str());
        SDL_Surface* surfaceRight = IMG_Load_RW(rwRight, 1);
        BR_ASSERT((surfaceRight != nullptr), "Create surface failed:%s", rightSide.c_str());

        SDL_RWops* rwLeft = SDL_RWFromFile(leftSide.c_str(), "rb");
        BR_ASSERT((rwLeft != nullptr), "Load cube texture failed:%s", leftSide.c_str());
        SDL_Surface* surfaceLeft = IMG_Load_RW(rwLeft, 1);
        BR_ASSERT((surfaceLeft != nullptr), "Create surface failed:%s", leftSide.c_str());

        SDL_RWops* rwTop = SDL_RWFromFile(topSide.c_str(), "rb");
        BR_ASSERT((rwTop != nullptr), "Load cube texture failed:%s", topSide.c_str());
        SDL_Surface* surfaceTop = IMG_Load_RW(rwTop, 1);
        BR_ASSERT((surfaceTop != nullptr), "Create surface failed:%s", topSide.c_str());

        SDL_RWops* rwBottom = SDL_RWFromFile(bottomSide.c_str(), "rb");
        BR_ASSERT((rwBottom != nullptr), "Load cube texture failed:%s", bottomSide.c_str());
        SDL_Surface* surfaceBottom = IMG_Load_RW(rwBottom, 1);
        BR_ASSERT((surfaceBottom != nullptr), "Create surface failed:%s", bottomSide.c_str());

        SDL_RWops* rwBack = SDL_RWFromFile(backSide.c_str(), "rb");
        BR_ASSERT((rwBack != nullptr), "Load cube texture failed:%s", backSide.c_str());
        SDL_Surface* surfaceBack = IMG_Load_RW(rwBack, 1);
        BR_ASSERT((surfaceBack != nullptr), "Create surface failed:%s", backSide.c_str());

        SDL_RWops* rwFront = SDL_RWFromFile(frontSide.c_str(), "rb");
        BR_ASSERT((rwFront != nullptr), "Load cube texture failed:%s", frontSide.c_str());
        SDL_Surface* surfaceFront = IMG_Load_RW(rwFront, 1);
        BR_ASSERT((surfaceFront != nullptr), "Create surface failed:%s", frontSide.c_str());

        BR_ASSERT(((surfaceRight->format->BytesPerPixel == 3 || surfaceRight->format->BytesPerPixel == 4) &&
                   (surfaceLeft->format->BytesPerPixel == 3 || surfaceLeft->format->BytesPerPixel == 4) &&
                   (surfaceTop->format->BytesPerPixel == 3 || surfaceTop->format->BytesPerPixel == 4) &&
                   (surfaceBottom->format->BytesPerPixel == 3 || surfaceBottom->format->BytesPerPixel == 4) &&
                   (surfaceBack->format->BytesPerPixel == 3 || surfaceBack->format->BytesPerPixel == 4) &&
                   (surfaceFront->format->BytesPerPixel == 3 || surfaceFront->format->BytesPerPixel == 4))
        , "Load cube texture failed:%s. Use 24 or 32 bit depth", folderPath.c_str());

        int pixelFormat = GL_RGB;
        if(4 == surfaceRight->format->BytesPerPixel) pixelFormat = GL_RGBA;

        glGenTextures(1, &m_openGLID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_openGLID);

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, pixelFormat, surfaceRight->w, surfaceRight->h, 0, pixelFormat, GL_UNSIGNED_BYTE, surfaceRight->pixels);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, pixelFormat, surfaceLeft->w, surfaceLeft->h, 0, pixelFormat, GL_UNSIGNED_BYTE, surfaceLeft->pixels);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, pixelFormat, surfaceTop->w, surfaceTop->h, 0, pixelFormat, GL_UNSIGNED_BYTE, surfaceTop->pixels);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, pixelFormat, surfaceBottom->w, surfaceBottom->h, 0, pixelFormat, GL_UNSIGNED_BYTE, surfaceBottom->pixels);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, pixelFormat, surfaceFront->w, surfaceFront->h, 0, pixelFormat, GL_UNSIGNED_BYTE, surfaceFront->pixels);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, pixelFormat, surfaceBack->w, surfaceBack->h, 0, pixelFormat, GL_UNSIGNED_BYTE, surfaceBack->pixels);

        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

        SDL_FreeSurface(surfaceRight);
        SDL_FreeSurface(surfaceLeft);
        SDL_FreeSurface(surfaceTop);
        SDL_FreeSurface(surfaceBottom);
        SDL_FreeSurface(surfaceBack);
        SDL_FreeSurface(surfaceFront);

        m_internalShader = Renderer::createShader("shaders/GLES/default/SkyBox.vert", "shaders/GLES/default/SkyBox.frag");
        m_internalShader->bind();
        m_internalShader->activateDiffuseTexture();

        std::vector<glm::vec3> vertices;
        vertices.emplace_back(glm::vec3{1.0f, -1.0f, -1.0f}); // right side +X
        vertices.emplace_back(glm::vec3{1.0f, -1.0f, 1.0f});
        vertices.emplace_back(glm::vec3{1.0f, 1.0f, -1.0f});
        vertices.emplace_back(glm::vec3{1.0f, 1.0f, 1.0f});

        vertices.emplace_back(glm::vec3{-1.0f, -1.0f, 1.0f}); // left side -X
        vertices.emplace_back(glm::vec3{-1.0f, -1.0f, -1.0f});
        vertices.emplace_back(glm::vec3{-1.0f, 1.0f, 1.0f});
        vertices.emplace_back(glm::vec3{-1.0f, 1.0f, -1.0f});

        vertices.emplace_back(glm::vec3{-1.0f, 1.0f, -1.0f}); // top +Y
        vertices.emplace_back(glm::vec3{1.0f, 1.0f, -1.0f});
        vertices.emplace_back(glm::vec3{-1.0f, 1.0f, 1.0f});
        vertices.emplace_back(glm::vec3{1.0f, 1.0f, 1.0f});

        vertices.emplace_back(glm::vec3{-1.0f, -1.0f, -1.0f}); // bottom -Y
        vertices.emplace_back(glm::vec3{1.0f, -1.0f, -1.0f});
        vertices.emplace_back(glm::vec3{-1.0f, -1.0f, 1.0f});
        vertices.emplace_back(glm::vec3{1.0f, -1.0f, 1.0f});

        vertices.emplace_back(glm::vec3{1.0f, -1.0f, 1.0f}); // front side +Z
        vertices.emplace_back(glm::vec3{-1.0f, -1.0f, 1.0f});
        vertices.emplace_back(glm::vec3{1.0f, 1.0f, 1.0f});
        vertices.emplace_back(glm::vec3{-1.0f, 1.0f, 1.0f});

        vertices.emplace_back(glm::vec3{-1.0f, -1.0f, -1.0f}); // back side -Z
        vertices.emplace_back(glm::vec3{1.0f, -1.0f, -1.0f});
        vertices.emplace_back(glm::vec3{-1.0f, 1.0f, -1.0f});
        vertices.emplace_back(glm::vec3{1.0f, 1.0f, -1.0f});
        m_vertexPosBuffer = Renderer::createVertexBuffer(vertices);

        std::vector<uint32_t> indices{0,1,2,    1,2,3, // two triangles
                                      4,5,6,    5,6,7,
                                      8,9,10,   9,10,11,
                                      12,13,14, 13,14,15,
                                      16,17,18, 17,18,19,
                                      20,21,22, 21,22,23};
        m_indexBuffer = Renderer::createIndexBuffer(indices);

        m_vertexArray = Renderer::createVertexArray();
        m_vertexArray->addVertexBuffer(m_vertexPosBuffer);
        m_vertexArray->setIndexBuffer(m_indexBuffer);
    }

    AndroidGLESSkyBox::~AndroidGLESSkyBox()
    {
        glDeleteTextures(1, &m_openGLID);
    }

    void AndroidGLESSkyBox::draw()
    {
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content

        if(useInternalShader)
        {
            m_internalShader->bind();
            m_persp = Camera::getProjection();
            m_view = glm::mat4(glm::mat3(Camera::getView())); // remove translation from matrix
            m_perspView = m_persp * m_view;
            m_internalShader->setMatrix4x4Float("VPMatrix", m_perspView);
        }

        if(GLESStateVariables::currentTexture0 != m_openGLID)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, m_openGLID);
            GLESStateVariables::currentTexture0 = m_openGLID;
        }

        m_vertexArray->bind();
        m_vertexArray->draw();

        glDepthFunc(GL_LESS); // set depth function back to default
    }
}