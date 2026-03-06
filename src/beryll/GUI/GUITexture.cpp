#include "GUITexture.h"
#include "beryll/renderer/Renderer.h"
#include "beryll/renderer/Camera.h"
#include "beryll/core/EventHandler.h"

namespace Beryll
{
    GUITexture::GUITexture(const char* texturePath,
                           const glm::vec3& pos, const glm::vec2& widthHeight, bool consumeDownEvent)
                           : GUIObject(pos, widthHeight), m_consumeEvent(consumeDownEvent)
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
        std::vector<Finger>& fingers = EventHandler::getFingers();
        for(Finger& f : fingers)
        {
            // Flipper Y for opengl.
            glm::vec2 flippedY = f.normalizedPos;
            flippedY.y = 1.0f - flippedY.y;

            if(flippedY.x > getPositionNormalized().x && flippedY.x < getPositionNormalized().x + getWidthHeightNormalized().x &&
               flippedY.y > getPositionNormalized().y && flippedY.y < getPositionNormalized().y + getWidthHeightNormalized().y)
            {
                // If any finger in button area.
                if(f.downEvent && !f.handled && m_consumeEvent)
                {
                    // Let image consume event if allowed.
                    f.handled = true;
                    BR_INFO("%s", "image consume event");
                }
            }
        }
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
