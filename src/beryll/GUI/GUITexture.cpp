#include "GUITexture.h"
#include "beryll/renderer/Renderer.h"
#include "beryll/renderer/Camera.h"

namespace Beryll
{
    GUITexture::GUITexture(const char* texturePath,
                           const glm::vec3& pos, const glm::vec2& widthHeight) : GUIObject(pos, widthHeight)
    {
        BR_ASSERT((texturePath != nullptr && texturePath[0] != '\0'), "%s", "Path to default texture can not be empty.");

        m_texture = Renderer::createTexture(texturePath, TextureType::DIFFUSE_TEXTURE_MAT_1);

        m_internalShader = Renderer::createShader(BeryllConstants::GUIElementWithTextureVertexPath.data(),
                                                  BeryllConstants::GUIElementWithTextureFragmentPath.data());
        m_internalShader->bind();
        m_internalShader->activateDiffuseTextureMat1();
        m_internalShader->unBind();
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
