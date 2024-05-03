#include "LoadingScreen.h"
#include "beryll/renderer/Renderer.h"
#include "beryll/renderer/Camera.h"
#include "beryll/core/Window.h"

namespace Beryll
{
    std::shared_ptr<VertexBuffer> LoadingScreen::m_vertexPosBuffer;
    std::shared_ptr<VertexBuffer> LoadingScreen::m_textureCoordsBuffer;
    std::shared_ptr<IndexBuffer> LoadingScreen::m_indexBuffer;
    std::unique_ptr<VertexArray> LoadingScreen::m_vertexArray;
    std::shared_ptr<Shader> LoadingScreen::m_internalShader;
    std::vector<std::unique_ptr<Texture>> LoadingScreen::m_diffuseTextures;

    float LoadingScreen::m_loadingProgress = 0.0f;
    int LoadingScreen::m_textureIndex = 0;
    bool LoadingScreen::m_created = false;

    void LoadingScreen::create()
    {
        if(m_created) { return; }

        std::vector<glm::vec3> vertices{glm::vec3(-1.0f, -1.0f, 1.0f),
                                        glm::vec3( 1.0f, -1.0f, 1.0f),
                                        glm::vec3( 1.0f,  1.0f, 1.0f),
                                        glm::vec3(-1.0f,  1.0f, 1.0f)};

#if defined(ANDROID)
        std::vector<glm::vec2> textureCoords{glm::vec2(0.0f, 1.0f), // Flipped Y for OpenGL.
                                             glm::vec2(1.0f, 1.0f),
                                             glm::vec2(1.0f, 0.0f),
                                             glm::vec2(0.0f, 0.0f)};
#elif defined(APPLE)

#endif

        std::vector<uint32_t> indices{0,1,2,
                                      2,3,0};

        m_vertexPosBuffer = Renderer::createStaticVertexBuffer(vertices);
        m_textureCoordsBuffer = Renderer::createStaticVertexBuffer(textureCoords);
        m_indexBuffer = Renderer::createStaticIndexBuffer(indices);

        m_vertexArray = Renderer::createVertexArray();
        m_vertexArray->addVertexBuffer(m_vertexPosBuffer);
        m_vertexArray->addVertexBuffer(m_textureCoordsBuffer);
        m_vertexArray->setIndexBuffer(m_indexBuffer);

        m_internalShader = Renderer::createShader(BeryllConstants::loadingScreenVertexPath.data(),
                                                  BeryllConstants::loadingScreenFragmentPath.data());
        m_internalShader->bind();
        m_internalShader->activateDiffuseTextureMat1();

        m_created = true;
    }

    void LoadingScreen::setTextures(const std::vector<std::string>& textures)
    {
        BR_ASSERT((!textures.empty()), "%s", "Loading screen must have at least 1 texture.");

        m_diffuseTextures.clear();

        for(const std::string& path : textures)
        {
            m_diffuseTextures.emplace_back(Renderer::createTexture(path.c_str(), TextureType::DIFFUSE_TEXTURE_MAT_1));
        }

        selectRandomTexture();
    }

    void LoadingScreen::showProgress(float pr)
    {
        BR_ASSERT((!m_diffuseTextures.empty()), "%s", "Loading screen textures are empty. Use setTextures().");

        BR_ASSERT((pr >= 0.0f && pr <= 100.0f), "%s", "Loading progress must be between 0.0f and 100.0f.");

        m_loadingProgress = pr * 0.01f; // Same as pr / 100.0f.

        Window::getInstance()->clear();

        m_internalShader->bind();
        m_internalShader->setMatrix4x4Float("VPMatrix", Camera::getLoadingScreenCamera());
        SDL_DisplayOrientation display = Window::getInstance()->currentOrientation;
        if(display == SDL_ORIENTATION_PORTRAIT || display == SDL_ORIENTATION_PORTRAIT_FLIPPED)
        {
            m_internalShader->set1Float("loadingProgressInScreenSpace", Window::getInstance()->getScreenHeight() * m_loadingProgress);
            m_internalShader->set1Int("displayPortraitOrientation", 1);
        }
        else
        {
            m_internalShader->set1Float("loadingProgressInScreenSpace", Window::getInstance()->getScreenWidth() * m_loadingProgress);
            m_internalShader->set1Int("displayPortraitOrientation", 0);
        }

        m_diffuseTextures[m_textureIndex]->bind();

        m_vertexArray->bind();
        m_vertexArray->draw();
        m_vertexArray->unBind();

        m_diffuseTextures[m_textureIndex]->unBind();

        m_internalShader->unBind();

        Window::getInstance()->swapWindow();
    }
}
