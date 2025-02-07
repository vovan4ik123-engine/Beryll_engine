#include "AndroidGLESSkyBox.h"
#include "beryll/core/Log.h"
#include "beryll/renderer/Camera.h"
#include "beryll/renderer/Renderer.h"
#include "beryll/platform/androidGLES/AndroidGLESGlobal.h"

#include <GLES3/gl32.h>
#include <GLES3/gl3ext.h>

namespace Beryll
{
    AndroidGLESSkyBox::AndroidGLESSkyBox(const std::string& folderPath)
    {
        std::string rightSide = folderPath + "/right.jpg";
        std::string leftSide = folderPath + "/left.jpg";
        std::string topSide = folderPath + "/top.jpg";
        std::string bottomSide = folderPath + "/bottom.jpg";
        std::string backSide = folderPath + "/back.jpg";
        std::string frontSide = folderPath + "/front.jpg";

        SDL_IOStream* rwRight = SDL_IOFromFile(rightSide.c_str(), "rb");
        BR_ASSERT((rwRight != nullptr), "Load cube texture failed: %s", rightSide.c_str());
        SDL_Surface* surfaceRight = IMG_Load_IO(rwRight, true);
        BR_ASSERT((surfaceRight != nullptr), "Create surface failed: %s", rightSide.c_str());

        SDL_IOStream* rwLeft = SDL_IOFromFile(leftSide.c_str(), "rb");
        BR_ASSERT((rwLeft != nullptr), "Load cube texture failed: %s", leftSide.c_str());
        SDL_Surface* surfaceLeft = IMG_Load_IO(rwLeft, true);
        BR_ASSERT((surfaceLeft != nullptr), "Create surface failed: %s", leftSide.c_str());

        SDL_IOStream* rwTop = SDL_IOFromFile(topSide.c_str(), "rb");
        BR_ASSERT((rwTop != nullptr), "Load cube texture failed: %s", topSide.c_str());
        SDL_Surface* surfaceTop = IMG_Load_IO(rwTop, true);
        BR_ASSERT((surfaceTop != nullptr), "Create surface failed: %s", topSide.c_str());

        SDL_IOStream* rwBottom = SDL_IOFromFile(bottomSide.c_str(), "rb");
        BR_ASSERT((rwBottom != nullptr), "Load cube texture failed: %s", bottomSide.c_str());
        SDL_Surface* surfaceBottom = IMG_Load_IO(rwBottom, true);
        BR_ASSERT((surfaceBottom != nullptr), "Create surface failed: %s", bottomSide.c_str());

        SDL_IOStream* rwBack = SDL_IOFromFile(backSide.c_str(), "rb");
        BR_ASSERT((rwBack != nullptr), "Load cube texture failed: %s", backSide.c_str());
        SDL_Surface* surfaceBack = IMG_Load_IO(rwBack, true);
        BR_ASSERT((surfaceBack != nullptr), "Create surface failed: %s", backSide.c_str());

        SDL_IOStream* rwFront = SDL_IOFromFile(frontSide.c_str(), "rb");
        BR_ASSERT((rwFront != nullptr), "Load cube texture failed: %s", frontSide.c_str());
        SDL_Surface* surfaceFront = IMG_Load_IO(rwFront, true);
        BR_ASSERT((surfaceFront != nullptr), "Create surface failed: %s", frontSide.c_str());

        BR_ASSERT(((SDL_BYTESPERPIXEL(surfaceRight->format) == 3  || SDL_BYTESPERPIXEL(surfaceRight->format) == 4) &&
                   (SDL_BYTESPERPIXEL(surfaceLeft->format) == 3   || SDL_BYTESPERPIXEL(surfaceLeft->format) == 4) &&
                   (SDL_BYTESPERPIXEL(surfaceTop->format) == 3    || SDL_BYTESPERPIXEL(surfaceTop->format) == 4) &&
                   (SDL_BYTESPERPIXEL(surfaceBottom->format) == 3 || SDL_BYTESPERPIXEL(surfaceBottom->format) == 4) &&
                   (SDL_BYTESPERPIXEL(surfaceBack->format) == 3   || SDL_BYTESPERPIXEL(surfaceBack->format) == 4) &&
                   (SDL_BYTESPERPIXEL(surfaceFront->format) == 3  || SDL_BYTESPERPIXEL(surfaceFront->format) == 4))
        , "Load cube texture failed: %s. Use 24 or 32 bit depth", folderPath.c_str());

        int pixelFormat = GL_RGB;
        if(SDL_BYTESPERPIXEL(surfaceRight->format) == 4)
            pixelFormat = GL_RGBA;

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

        SDL_DestroySurface(surfaceRight);
        SDL_DestroySurface(surfaceLeft);
        SDL_DestroySurface(surfaceTop);
        SDL_DestroySurface(surfaceBottom);
        SDL_DestroySurface(surfaceBack);
        SDL_DestroySurface(surfaceFront);

        m_internalShader = Renderer::createShader(BeryllConstants::skyBoxVertexPath.data(),
                                                  BeryllConstants::skyBoxFragmentPath.data());
        m_internalShader->bind();
        m_internalShader->activateSkyBoxTexture();

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
        m_vertexPosBuffer = Renderer::createStaticVertexBuffer(vertices);

        // Clockwise winding order (because we are always inside cube)
        // for correctly draw sky box if face culling enabled.
        std::vector<uint32_t> indices{0,1,2,    1,3,2, // Two triangles.
                                      4,5,6,    5,7,6,
                                      8,9,10,   9,11,10,
                                      12,14,13, 13,14,15,
                                      16,17,18, 17,19,18,
                                      20,21,22, 21,23,22};
        m_indexBuffer = Renderer::createStaticIndexBuffer(indices);

        m_vertexArray = Renderer::createVertexArray();
        m_vertexArray->addVertexBuffer(m_vertexPosBuffer);
        m_vertexArray->setIndexBuffer(m_indexBuffer);
    }

    AndroidGLESSkyBox::~AndroidGLESSkyBox()
    {
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        GLESStateVariables::currentSkyBoxTextureID7 = 0;
        glDeleteTextures(1, &m_openGLID);
    }

    void AndroidGLESSkyBox::draw()
    {
        if(GLESStateVariables::currentSkyBoxTextureID7 != m_openGLID)
        {
            glActiveTexture(GL_TEXTURE7);
            glBindTexture(GL_TEXTURE_CUBE_MAP, m_openGLID);
            GLESStateVariables::currentSkyBoxTextureID7 = m_openGLID;
        }

        glDepthFunc(GL_LEQUAL);  // Change depth function so depth test passes when values are equal to depth buffer's content.

        m_internalShader->bind();
        m_persp = Camera::getProjection();
        m_view = glm::mat4(glm::mat3(Camera::getView())); // Remove translation from matrix.
        m_perspView = m_persp * m_view;
        m_internalShader->setMatrix4x4Float("VPMatrix", m_perspView);

        m_vertexArray->bind();
        m_vertexArray->draw();

        glDepthFunc(GL_LESS); // Set depth function back to default.
    }
}
