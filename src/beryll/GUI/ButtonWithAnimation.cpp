#include "ButtonWithAnimation.h"
#include "beryll/renderer/Renderer.h"
#include "beryll/renderer/Camera.h"
#include "beryll/core/EventHandler.h"
#include "beryll/core/TimeStep.h"

namespace Beryll
{
    ButtonWithAnimation::ButtonWithAnimation(const char* texturesPath, const std::vector<const char*> texturesNames,
                                             const float animDurationSec, bool repeatAnimation,
                                             const glm::vec3& pos, const glm::vec2& widthHeight, bool actRepeat) : GUIObject(pos, widthHeight)
    {
        BR_ASSERT((texturesPath != nullptr && texturesPath[0] != '\0'), "%s", "Path to default texture can not be empty.");
        BR_ASSERT((texturesNames.empty() == false), "%s", "No textures names.");

        m_actRepeat = actRepeat;

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
        std::vector<Finger>& fingers = EventHandler::getFingers();

        if(fingers.empty())
        {
            m_pressed = false;
            m_touched = false;
            m_pressedFingerID = -100;
            m_isPressedFingerStillOnScreen = false;
        }
        else
        {
            if(m_actRepeat && m_pressed)
            {
                m_pressed = false;
                for(const Finger& f : fingers)
                {
                    // Flipper Y for opengl.
                    glm::vec2 flippedY = f.normalizedPos;
                    flippedY.y = 1.0f - flippedY.y;

                    if(flippedY.x > getPositionNormalized().x && flippedY.x < getPositionNormalized().x + getWidthHeightNormalized().x &&
                       flippedY.y > getPositionNormalized().y && flippedY.y < getPositionNormalized().y + getWidthHeightNormalized().y)
                    {
                        // If any finger in button area.
                        m_pressed = true;
                    }
                }
            }
            else
            {
                m_pressed = false;
            }

            m_touched = false;
            for(Finger& f : fingers)
            {
                // Flipper Y for opengl.
                glm::vec2 flippedY = f.normalizedPos;
                flippedY.y = 1.0f - flippedY.y;

                if(flippedY.x > getPositionNormalized().x && flippedY.x < getPositionNormalized().x + getWidthHeightNormalized().x &&
                   flippedY.y > getPositionNormalized().y && flippedY.y < getPositionNormalized().y + getWidthHeightNormalized().y)
                {
                    // If any finger in button area.
                    if(f.ID == m_pressedFingerID)
                        m_touched = true;

                    if(f.downEvent && !f.handled)
                    {
                        f.handled = true;
                        m_pressed = true;
                        m_pressedFingerID = f.ID;
                        m_isPressedFingerStillOnScreen = true;
                    }
                }
            }

            m_isPressedFingerStillOnScreen = false;
            for(const Finger& f : fingers)
            {
                if(f.ID == m_pressedFingerID)
                    m_isPressedFingerStillOnScreen = true;
            }

            if(!m_isPressedFingerStillOnScreen)
                m_pressedFingerID = -100;
        }

        if(m_pressed && m_action)
        {
            m_action();
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
