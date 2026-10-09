#include "ButtonWithTexture.h"
#include "beryll/renderer/Renderer.h"
#include "beryll/renderer/Camera.h"
#include "beryll/core/EventHandler.h"

namespace Beryll
{
    ButtonWithTexture::ButtonWithTexture(const char* defaultTexturePath,
                                         const char* touchedTexturePath,
                                         const glm::vec3& pos, const glm::vec2& widthHeight, bool actOnTouch, bool actRepeat, bool consumeDownEvent)
                                         : GUIObject(pos, widthHeight, actOnTouch, consumeDownEvent), m_actRepeat(actRepeat)
    {
        BR_ASSERT((defaultTexturePath != nullptr && defaultTexturePath[0] != '\0'), "%s", "Path to default texture can not be empty.");

        m_defaultTexture = Renderer::createTexture(defaultTexturePath, TextureType::DIFFUSE_TEXTURE_MAT_1);

        BR_INFO("touchedTexturePath %s", touchedTexturePath);
        if(touchedTexturePath != nullptr && touchedTexturePath[0] != '\0')
            m_touchedTexture = Renderer::createTexture(touchedTexturePath, TextureType::DIFFUSE_TEXTURE_MAT_1);

        m_internalShader = Renderer::createShader(BeryllConstants::GUIElementWithTextureVertexPath.data(),
                                                  BeryllConstants::GUIElementWithTextureFragmentPath.data());
        m_internalShader->bind();
        m_internalShader->activateDiffuseTextureMat1();
        m_internalShader->unBind();
    }

    ButtonWithTexture::~ButtonWithTexture()
    {

    }

    void ButtonWithTexture::updateBeforePhysics()
    {
        if(m_actRepeat && m_pressed && m_touchedFingerStillOnScreen)
        {
            m_pressed = true;
            m_touched = true;
        }
        else
        {
            m_pressed = false;
            m_touched = false;
        }

        m_touchedFingerStillOnScreen = false;
        std::vector<Finger>& fingers = EventHandler::getFingers();
        for(Finger& f : fingers)
        {
            if(f.normalizedPos.x > getPositionNormalized().x && f.normalizedPos.x < getPositionNormalized().x + getWidthHeightNormalized().x &&
               f.normalizedPos.y > getPositionNormalized().y && f.normalizedPos.y < getPositionNormalized().y + getWidthHeightNormalized().y)
            {
                // Finger is on screen and inside button area.
                if(f.downEvent)
                {
                    m_touchedFingerID = f.ID;

                    if(m_consumeEvent)
                        f.downEvent = false;

                    if(m_actOnTouch)
                        m_pressed = true;
                }

                if(f.ID == m_touchedFingerID)
                {
                    m_touchedFingerStillOnScreen = true;
                    m_touchedFingerStillInsideElement = true;
                }
            }
            else if(f.ID == m_touchedFingerID)
            {
                // Touched finger is on screen but outside button area.
                m_touchedFingerStillOnScreen = true;
                m_touchedFingerStillInsideElement = false;
            }
        }

        if(m_touchedFingerStillOnScreen && m_touchedFingerStillInsideElement)
            m_touched = true;

        if(!m_actOnTouch &&
           !m_touchedFingerStillOnScreen && m_touchedFingerStillInsideElement)
        {
            // Touched finger was released when it was inside element.
            m_pressed = true;
        }

        if(!m_touchedFingerStillOnScreen)
        {
            m_touchedFingerID = -100;
            m_touchedFingerStillInsideElement = false;
        }
    }

    void ButtonWithTexture::updateAfterPhysics()
    {

    }

    void ButtonWithTexture::draw()
    {
        m_internalShader->bind();
        m_internalShader->setMatrix4x4Float("VPMatrix", Camera::getCameraGUI());

        if(m_touched && m_touchedTexture)
            m_touchedTexture->bind();
        else
            m_defaultTexture->bind();

        m_vertexArray->bind();
        m_vertexArray->draw();
    }
}
