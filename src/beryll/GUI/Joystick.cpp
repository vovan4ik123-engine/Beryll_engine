#include "Joystick.h"
#include "beryll/renderer/Renderer.h"
#include "beryll/renderer/Camera.h"
#include "beryll/core/EventHandler.h"

namespace Beryll
{
    Joystick::Joystick(const char*  defaultTexturePath,
                       const char*  touchedTexturePath,
                       const glm::vec3& pos, const glm::vec2& widthHeight, bool consumeDownEvent)
                       : GUIObject(pos, widthHeight, consumeDownEvent)
    {
        BR_ASSERT((defaultTexturePath != nullptr && defaultTexturePath[0] != '\0'), "%s", "Path to default texture can not be empty.");

        m_defaultTexture = Renderer::createTexture(defaultTexturePath, TextureType::DIFFUSE_TEXTURE_MAT_1);

        if(touchedTexturePath != nullptr && touchedTexturePath[0] != '\0')
            m_touchedTexture = Renderer::createTexture(touchedTexturePath, TextureType::DIFFUSE_TEXTURE_MAT_1);

        m_internalShader = Renderer::createShader(BeryllConstants::GUIElementWithTextureVertexPath.data(),
                                                  BeryllConstants::GUIElementWithTextureFragmentPath.data());
        m_internalShader->bind();
        m_internalShader->activateDiffuseTextureMat1();
        m_internalShader->unBind();

        m_originNormalized.x = getPositionNormalized().x + (getWidthHeightNormalized().x * 0.5f);
        m_originNormalized.y = getPositionNormalized().y + (getWidthHeightNormalized().y * 0.5f);
    }

    Joystick::~Joystick()
    {

    }

    void Joystick::updateBeforePhysics()
    {
        m_touched = false;
        m_touchedDirectionFromOrigin.x = 0.0f;
        m_touchedDirectionFromOrigin.y = 0.0f;

        std::vector<Finger>& fingers = EventHandler::getFingers();
        for(Finger& f : fingers)
        {
            if(f.normalizedPos.x > getPositionNormalized().x && f.normalizedPos.x < getPositionNormalized().x + getWidthHeightNormalized().x &&
               f.normalizedPos.y > getPositionNormalized().y && f.normalizedPos.y < getPositionNormalized().y + getWidthHeightNormalized().y)
            {
                // If any finger in joystick area.
                if(f.downEvent)
                {
                    if(pressedFingerID == -100)
                        pressedFingerID = f.ID;

                    if(m_consumeEvent)
                        f.downEvent = false;
                }

                if(pressedFingerID == f.ID)
                {
                    m_touched = true;

                    glm::vec2 fingerDir = f.normalizedPos - m_originNormalized;
                    if(glm::length(fingerDir) > 0.001f)
                    {
                        if(Window::getInstance()->getScreenWidth() > Window::getInstance()->getScreenHeight())
                            fingerDir.x = fingerDir.x * Window::getInstance()->getScreenAspectRatio();
                        else
                            fingerDir.y = fingerDir.y * Window::getInstance()->getScreenAspectRatio();

                        m_touchedDirectionFromOrigin = glm::normalize(fingerDir);
                    }
                }
            }
        }

        if(!m_touched)
            pressedFingerID = -100;
    }

    void Joystick::updateAfterPhysics()
    {

    }

    void Joystick::draw()
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

    void Joystick::setOrigin(const glm::vec2 origInRange0to1)
    {
        BR_ASSERT((origInRange0to1.x >= 0.0f && origInRange0to1.x <= 1.0f &&
                   origInRange0to1.y >= 0.0f && origInRange0to1.y <= 1.0f ), "%s", "Joystick new origin not correct.");

        m_originNormalized = origInRange0to1;

        glm::vec3 newPos = getPositionNormalized();
        glm::vec2 currentWidthHeight = getWidthHeightNormalized();

        newPos.x = (origInRange0to1.x - (currentWidthHeight.x * 0.5f)) * 100.0f;
        newPos.y = (origInRange0to1.y - (currentWidthHeight.y * 0.5f)) * 100.0f;

        updatePositionInPercents(newPos);
    }
}
