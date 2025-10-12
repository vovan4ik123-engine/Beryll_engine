#include "ButtonWithAnimation.h"
#include "beryll/renderer/Renderer.h"
#include "beryll/core/EventHandler.h"
#include "MainImGUI.h"
#include "beryll/core/TimeStep.h"

namespace Beryll
{
    ButtonWithAnimation::ButtonWithAnimation(const std::string texturesPath, const std::vector<const std::string> texturesNames,
                                             const float animDurationSec, bool repeatAnimation,
                                             float l, float t, float w, float h, bool actRepeat, bool bringToFrontOnFocus)
    {
        BR_ASSERT((texturesPath.empty() == false), "%s", "Path to default texture can not be empty.");
        BR_ASSERT((texturesNames.empty() == false), "%s", "No textures names.");

        leftPos = l;
        topPos = t;
        width = w;
        height = h;

        m_actRepeat = actRepeat;

        if(!bringToFrontOnFocus)
        {
            m_noBackgroundNoFrame = m_noBackgroundNoFrame | ImGuiWindowFlags_NoBringToFrontOnFocus;
            m_noFrame = m_noFrame | ImGuiWindowFlags_NoBringToFrontOnFocus;
        }

        m_animationFrames.reserve(texturesNames.size());
        std::string pathAndName;
        for(const std::string& name : texturesNames)
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
                    if(f.normalizedPos.x > leftPos && f.normalizedPos.x < leftPos + width &&
                       f.normalizedPos.y > topPos && f.normalizedPos.y < topPos + height)
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
                if(f.normalizedPos.x > leftPos && f.normalizedPos.x < leftPos + width &&
                   f.normalizedPos.y > topPos && f.normalizedPos.y < topPos + height)
                {
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

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

        ImGui::SetNextWindowPos(ImVec2(leftPos * MainImGUI::getInstance()->getGUIWidth(), topPos * MainImGUI::getInstance()->getGUIHeight()));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f)); // Set next window size. Set axis to 0.0f to force an auto-fit on this axis.

        ImGui::Begin(m_IDAsString.c_str(), nullptr, m_noBackgroundNoFrame);

        ImGui::ImageButton(m_IDAsString.c_str(),
                           static_cast<ImTextureID>(m_animationFrames[m_currentFrameIndex]->getID()),
                           ImVec2(width * MainImGUI::getInstance()->getGUIWidth(), height * MainImGUI::getInstance()->getGUIHeight()));

        ImGui::End();

        ImGui::PopStyleColor(3);
    }
}
