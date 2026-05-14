#include "LoadingScreen.h"
#include "beryll/renderer/Renderer.h"
#include "beryll/renderer/Camera.h"
#include "beryll/core/Window.h"

namespace Beryll
{
    bool LoadingScreen::m_created = false;

    std::shared_ptr<VertexBuffer> LoadingScreen::m_vertexPosBuffer;
    std::shared_ptr<VertexBuffer> LoadingScreen::m_textureCoordsBuffer;
    std::shared_ptr<IndexBuffer> LoadingScreen::m_indexBuffer;
    std::unique_ptr<VertexArray> LoadingScreen::m_vertexArray;
    std::shared_ptr<Shader> LoadingScreen::m_internalShader;
    std::vector<std::unique_ptr<Texture>> LoadingScreen::m_diffuseTextures;

    int LoadingScreen::m_textureIndex = 0;

    void LoadingScreen::create()
    {
        if(m_created) { return; }

#if defined(ANDROID)
        std::vector<glm::vec3> vertices{glm::vec3{-1.0f, -1.0f, 1.0f},
                                        glm::vec3{ 1.0f, -1.0f, 1.0f},
                                        glm::vec3{ 1.0f,  1.0f, 1.0f},
                                        glm::vec3{-1.0f,  1.0f, 1.0f}};

        std::vector<glm::vec2> textureCoords{glm::vec2{0.0f, 1.0f}, // Flipped Y for OpenGL.
                                             glm::vec2{1.0f, 1.0f},
                                             glm::vec2{1.0f, 0.0f},
                                             glm::vec2{0.0f, 0.0f}};

        std::vector<uint32_t> indices{0,1,2,
                                      2,3,0};
#elif defined(APPLE)

#endif

        m_vertexPosBuffer = Renderer::createStaticVertexBuffer(vertices);
        m_textureCoordsBuffer = Renderer::createStaticVertexBuffer(textureCoords);
        m_indexBuffer = Renderer::createStaticIndexBuffer(indices);

        m_vertexArray = Renderer::createVertexArray();
        m_vertexArray->addVertexBuffer(m_vertexPosBuffer);
        m_vertexArray->addVertexBuffer(m_textureCoordsBuffer);
        m_vertexArray->setIndexBuffer(m_indexBuffer);

        m_internalShader = Renderer::createShader(BeryllConstants::GUIElementWithTextureVertexPath.data(),
                                                  BeryllConstants::GUIElementWithTextureFragmentPath.data());
        m_internalShader->bind();
        m_internalShader->activateDiffuseTextureMat1();
        m_internalShader->unBind();

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

    void LoadingScreen::show()
    {
        BR_ASSERT((!m_diffuseTextures.empty()), "%s", "Loading screen textures are empty. Use setTextures().");

        Window::getInstance()->clear();

        m_internalShader->bind();
        m_internalShader->setMatrix4x4Float("VPMatrix", Camera::getCameraGUI());

        m_diffuseTextures[m_textureIndex]->bind();

        m_vertexArray->bind();
        m_vertexArray->draw();
        m_vertexArray->unBind();

        m_diffuseTextures[m_textureIndex]->unBind();

        m_internalShader->unBind();

        Window::getInstance()->swapWindow();
    }
}
