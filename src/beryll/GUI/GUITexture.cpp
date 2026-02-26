#include "GUITexture.h"
#include "beryll/renderer/Renderer.h"
#include "beryll/renderer/Camera.h"

namespace Beryll
{
    GUITexture::GUITexture(const char* texturePath,
                           const glm::vec3& pos, const glm::vec2& widthHeight)
    {
        BR_ASSERT((texturePath != nullptr && texturePath[0] != '\0'), "%s", "Path to default texture can not be empty.");

        setPositionInPercents(pos);
        setWidthHeightInPercents(widthHeight);

        m_texture = Renderer::createTexture(texturePath, TextureType::DIFFUSE_TEXTURE_MAT_1);

#if defined(ANDROID)
        // Vertices created as dynamic buffer. Will be updated in updateBuffersWithPositions().
        std::vector<glm::vec3> vertices{glm::vec3(0.0f, 0.0f, 0.0f),
                                        glm::vec3(0.0f, 0.0f, 0.0f),
                                        glm::vec3(0.0f, 0.0f, 0.0f),
                                        glm::vec3(0.0f, 0.0f, 0.0f)};

        std::vector<glm::vec2> textureCoords{glm::vec2(0.0f, 1.0f), // Flipped Y for OpenGL.
                                             glm::vec2(1.0f, 1.0f),
                                             glm::vec2(1.0f, 0.0f),
                                             glm::vec2(0.0f, 0.0f)};

        std::vector<uint32_t> indices{0,1,2,
                                      2,3,0};
#elif defined(APPLE)

#endif

        m_vertexPosBuffer = Renderer::createDynamicVertexBuffer(VertexAttribType::FLOAT, VertexAttribSize::THREE, sizeof(glm::vec3) * vertices.size());
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

        updateBuffersWithPositions(); // Only after buffers created.
    }

    GUITexture::~GUITexture()
    {

    }

    void GUITexture::updateBeforePhysics()
    {

    }

    void GUITexture::updateAfterPhysics()
    {

    }

    void GUITexture::draw()
    {
        m_internalShader->bind();
        m_internalShader->setMatrix4x4Float("VPMatrix", Camera::getCameraGUI());

        m_texture->bind();

        m_vertexArray->bind();
        m_vertexArray->draw();
    }
}
