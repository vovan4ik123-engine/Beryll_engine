#include "Joystick.h"
#include "beryll/renderer/Renderer.h"
#include "beryll/core/EventHandler.h"
#include "MainImGUI.h"

namespace Beryll
{
    Joystick::Joystick(const std::string& defaultTexturePath,
                       const std::string& touchedTexturePath,
                       float l, float t, float w, float h, bool bringToFrontOnFocus)
    {
        BR_ASSERT((defaultTexturePath.empty() == false), "%s", "Path to default texture can not be empty.");

        leftPos = l;
        topPos = t;
        width = w;
        height = h;

        if(!bringToFrontOnFocus)
        {
            m_noBackgroundNoFrame = m_noBackgroundNoFrame | ImGuiWindowFlags_NoBringToFrontOnFocus;
            m_noFrame = m_noFrame | ImGuiWindowFlags_NoBringToFrontOnFocus;
        }

        m_defaultTexture = Renderer::createTexture(defaultTexturePath.c_str(), TextureType::DIFFUSE_TEXTURE_MAT_1);

        if( !touchedTexturePath.empty())
            m_touchedTexture = Renderer::createTexture(touchedTexturePath.c_str(), TextureType::DIFFUSE_TEXTURE_MAT_1);

        const float leftPosPixels = leftPos * MainImGUI::getInstance()->getGUIWidth();
        const float rightPosPixels = leftPosPixels + (MainImGUI::getInstance()->getGUIWidth() * width);
        m_joystickOriginInPixels.x = leftPosPixels + ((rightPosPixels - leftPosPixels) * 0.5f);

        const float topPosPixels = topPos * MainImGUI::getInstance()->getGUIHeight();
        const float bottomPosPixels = topPosPixels + (height * MainImGUI::getInstance()->getGUIHeight());
        m_joystickOriginInPixels.y = topPosPixels + ((bottomPosPixels - topPosPixels) * 0.5f);
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
            if(f.normalizedPos.x > leftPos && f.normalizedPos.x < leftPos + width &&
               f.normalizedPos.y > topPos && f.normalizedPos.y < topPos + height)
            {
                // If any finger in joystick area.
                if(!f.handled)
                    f.handled = true;

                m_touched = true;

                const glm::vec2 touchDistanceFromOriginInPixels = f.ImGuiScreenPos - m_joystickOriginInPixels;
                if(glm::length(touchDistanceFromOriginInPixels) > 0.001f)
                {
                    m_touchedDirectionFromOrigin = glm::normalize(touchDistanceFromOriginInPixels);
                    m_touchedDirectionFromOrigin.y = -m_touchedDirectionFromOrigin.y;
                }
            }
        }
    }

    void Joystick::updateAfterPhysics()
    {

    }

    void Joystick::draw()
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
                               reinterpret_cast<ImTextureID>(ImU64(m_touchedTexture->getID())),
                               ImVec2(width * MainImGUI::getInstance()->getGUIWidth(), height * MainImGUI::getInstance()->getGUIHeight()));
        }
        else
        {
            ImGui::ImageButton(m_IDAsString.c_str(),
                               reinterpret_cast<ImTextureID>(ImU64(m_defaultTexture->getID())),
                               ImVec2(width * MainImGUI::getInstance()->getGUIWidth(), height * MainImGUI::getInstance()->getGUIHeight()));
        }

        ImGui::End();

        ImGui::PopStyleColor(3);
    }

    void Joystick::setOrigin(glm::vec2 origInRange0to1)
    {
        BR_ASSERT((origInRange0to1.x >= 0.0f &&
                   origInRange0to1.x <= 1.0f &&
                   origInRange0to1.y >= 0.0f &&
                   origInRange0to1.y <= 1.0f ), "%s", "Joystick new origin not correct.");

        m_joystickOriginInPixels.x = origInRange0to1.x * MainImGUI::getInstance()->getGUIWidth();
        m_joystickOriginInPixels.y = origInRange0to1.y * MainImGUI::getInstance()->getGUIHeight();

        leftPos = origInRange0to1.x - (width * 0.5f);
        topPos = origInRange0to1.y - (height * 0.5f);
    }
}
