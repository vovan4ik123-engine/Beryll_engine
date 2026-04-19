#include "CheckBox.h"
#include "beryll/renderer/Renderer.h"
#include "beryll/renderer/Camera.h"
#include "beryll/core/EventHandler.h"

namespace Beryll
{
    CheckBox::CheckBox(const char* unMarkedTexturePath,
                       const char* markedTexturePath,
                       const glm::vec3& pos, const glm::vec2& widthHeight, bool consumeDownEvent)
                       : GUIObject(pos, widthHeight, consumeDownEvent)
    {
        BR_ASSERT((unMarkedTexturePath != nullptr && unMarkedTexturePath[0] != '\0'), "%s", "Path to unMarked Texture can not be empty.");
        BR_ASSERT((markedTexturePath != nullptr && markedTexturePath[0] != '\0'), "%s", "Path to marked Texture can not be empty.");

        m_unMarkedTexture = Renderer::createTexture(unMarkedTexturePath, TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_markedTexture = Renderer::createTexture(markedTexturePath, TextureType::DIFFUSE_TEXTURE_MAT_1);

        m_internalShader = Renderer::createShader(BeryllConstants::GUIElementWithTextureVertexPath.data(),
                                                  BeryllConstants::GUIElementWithTextureFragmentPath.data());
        m_internalShader->bind();
        m_internalShader->activateDiffuseTextureMat1();
        m_internalShader->unBind();
    }

    CheckBox::~CheckBox()
    {

    }

    void CheckBox::updateBeforePhysics()
    {
        m_marking = false;
        m_unMarking = false;

        std::vector<Finger>& fingers = EventHandler::getFingers();
        for(Finger& f : fingers)
        {
            if(f.normalizedPos.x > getPositionNormalized().x && f.normalizedPos.x < getPositionNormalized().x + getWidthHeightNormalized().x &&
               f.normalizedPos.y > getPositionNormalized().y && f.normalizedPos.y < getPositionNormalized().y + getWidthHeightNormalized().y)
            {
                // If any finger in checkbox area.
                if(f.downEvent)
                {
                    if(marked)
                    {
                        marked = false;
                        m_marking = false;
                        m_unMarking = true;
                    }
                    else
                    {
                        marked = true;
                        m_marking = true;
                        m_unMarking = false;
                    }

                    if(m_consumeEvent)
                        f.downEvent = false;
                }
            }
        }

        if(m_marking && m_action)
        {
            m_action();
        }
    }

    void CheckBox::updateAfterPhysics()
    {

    }

    void CheckBox::draw()
    {
        m_internalShader->bind();
        m_internalShader->setMatrix4x4Float("VPMatrix", Camera::getCameraGUI());

        if(marked)
            m_markedTexture->bind();
        else
            m_unMarkedTexture->bind();

        m_vertexArray->bind();
        m_vertexArray->draw();
    }
}
