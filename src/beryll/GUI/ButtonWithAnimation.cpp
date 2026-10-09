#include "ButtonWithAnimation.h"
#include "beryll/renderer/Renderer.h"
#include "beryll/renderer/Camera.h"
#include "beryll/core/EventHandler.h"
#include "beryll/core/TimeStep.h"

namespace Beryll
{
    ButtonWithAnimation::ButtonWithAnimation(const char* texturesPath, const std::vector<const char*> texturesNames,
                                             const float animDurationSec, bool repeatAnimation,
                                             const glm::vec3& pos, const glm::vec2& widthHeight, bool actOnTouch, bool actRepeat, bool consumeDownEvent)
                                             : GUIObject(pos, widthHeight, actOnTouch, consumeDownEvent), m_actRepeat(actRepeat)
    {
        BR_ASSERT((texturesPath != nullptr && texturesPath[0] != '\0'), "%s", "Path to default texture can not be empty.");
        BR_ASSERT((texturesNames.empty() == false), "%s", "No textures names.");

        m_animationFrames.reserve(texturesNames.size());
        std::string pathAndName;
        for(const char* name : texturesNames)
        {
            pathAndName = texturesPath;
            pathAndName += '/';
            pathAndName += name;
            m_animationFrames.push_back(Beryll::Renderer::createTexture(pathAndName.c_str(), Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1));
        }

        m_animationTotalDuration = animDurationSec;
        m_repeatAnimation = repeatAnimation;
        m_animationFinished = false;
        m_currentFrameIndex = 0;
        m_timeOfOneFrame = m_animationTotalDuration / float(m_animationFrames.size());
        m_animationStartTime = Beryll::TimeStep::getSecFromStart();

        m_internalShader = Renderer::createShader(BeryllConstants::GUIElementWithTextureVertexPath.data(),
                                                  BeryllConstants::GUIElementWithTextureFragmentPath.data());
        m_internalShader->bind();
        m_internalShader->activateDiffuseTextureMat1();
        m_internalShader->unBind();
    }

    ButtonWithAnimation::~ButtonWithAnimation()
    {

    }

    void ButtonWithAnimation::enable(bool restartAnim)
    {
        GUIObject::enable();

        if(restartAnim)
        {
            m_animationStartTime = Beryll::TimeStep::getSecFromStart();
            m_animationFinished = false;
            BR_INFO("%s", "restartAnim");
        }
    }

    void ButtonWithAnimation::updateBeforePhysics()
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

    void ButtonWithAnimation::updateAfterPhysics()
    {

    }

    void ButtonWithAnimation::draw()
    {
        m_animationFinished = true;
        m_currentFrameIndex = m_animationFrames.size() - 1;

        if(m_repeatAnimation ||
           m_animationStartTime + m_animationTotalDuration > Beryll::TimeStep::getSecFromStart())
        {
            m_animationCurrentTime = std::fmodf(Beryll::TimeStep::getSecFromStart() - m_animationStartTime, m_animationTotalDuration);
            m_animationFinished = false;
            m_currentFrameIndex = int(m_animationCurrentTime / m_timeOfOneFrame);

            if(m_currentFrameIndex >= m_animationFrames.size())
                m_currentFrameIndex = m_animationFrames.size() - 1;
        }

        m_internalShader->bind();
        m_internalShader->setMatrix4x4Float("VPMatrix", Camera::getCameraGUI());

        m_animationFrames[m_currentFrameIndex]->bind();

        m_vertexArray->bind();
        m_vertexArray->draw();
    }
}
