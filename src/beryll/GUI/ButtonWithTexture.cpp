#include "ButtonWithTexture.h"
#include "beryll/renderer/Renderer.h"
#include "beryll/core/EventHandler.h"
#include "MainImGUI.h"

namespace Beryll
{
    ButtonWithTexture::ButtonWithTexture(const std::string& defaultTexturePath,
                                         const std::string& touchedTexturePath,
                                         float l, float t, float w, float h, bool actRepeat, bool bringToFrontOnFocus)
    {
        BR_ASSERT((defaultTexturePath.empty() == false), "%s", "Path to default texture can not be empty.");

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

        m_defaultTexture = Renderer::createTexture(defaultTexturePath.c_str(), TextureType::DIFFUSE_TEXTURE_MAT_1);

        if( !touchedTexturePath.empty())
            m_touchedTexture = Renderer::createTexture(touchedTexturePath.c_str(), TextureType::DIFFUSE_TEXTURE_MAT_1);
    }

    ButtonWithTexture::~ButtonWithTexture()
    {

    }

    void ButtonWithTexture::updateBeforePhysics()
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
            if(m_actRepeat && m_pressed) // still keep button pressed or not
            {
                m_pressed = false;
                for(const Finger& f : fingers)
                {
                    if(f.normalizedPos.x > leftPos && f.normalizedPos.x < leftPos + width &&
                       f.normalizedPos.y > topPos && f.normalizedPos.y < topPos + height)
                    {
                        // if any finger in button area
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

    void ButtonWithTexture::updateAfterPhysics()
    {

    }

    void ButtonWithTexture::draw()
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

        ImGui::SetNextWindowPos(ImVec2(leftPos * MainImGUI::getInstance()->getGUIWidth(), topPos * MainImGUI::getInstance()->getGUIHeight()));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f)); // Set next window size. Set axis to 0.0f to force an auto-fit on this axis.

        ImGui::Begin(m_IDAsString.c_str(), nullptr, m_noBackgroundNoFrame);

        if(m_touched && m_touchedTexture)
        {
            ImGui::ImageButton(m_IDAsString.c_str(),
                               reinterpret_cast<ImTextureID>(m_touchedTexture->getID()),
                               ImVec2(width * MainImGUI::getInstance()->getGUIWidth(), height * MainImGUI::getInstance()->getGUIHeight()));
        }
        else
        {
            ImGui::ImageButton(m_IDAsString.c_str(),
                               reinterpret_cast<ImTextureID>(m_defaultTexture->getID()),
                               ImVec2(width * MainImGUI::getInstance()->getGUIWidth(), height * MainImGUI::getInstance()->getGUIHeight()));
        }

        ImGui::End();

        ImGui::PopStyleColor(3);
    }
}
