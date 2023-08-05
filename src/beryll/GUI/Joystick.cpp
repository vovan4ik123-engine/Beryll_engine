#include "Joystick.h"
#include "beryll/renderer/Renderer.h"
#include "beryll/core/EventHandler.h"
#include "MainImGUI.h"

namespace Beryll
{
    Joystick::Joystick(const std::string& defaultTexturePath,
                       const std::string& touchedTexturePath,
                       float left, float top, float width, float height)
    {
        BR_ASSERT((defaultTexturePath.empty() == false), "%s", "Path to default texture can not be empty");

        m_leftPos = left / 100.0f;
        m_topPos = top / 100.0f;
        m_width = width / 100.0f;
        m_height = height / 100.0f;

        m_defaultTexture = Renderer::createTexture(defaultTexturePath.c_str(), TextureType::DIFFUSE_TEXTURE);

        if( !touchedTexturePath.empty())
            m_touchedTexture = Renderer::createTexture(touchedTexturePath.c_str(), TextureType::DIFFUSE_TEXTURE);

        float leftPosPixels = m_leftPos * MainImGUI::getInstance()->getGUIWidth();
        float rightPosPixels = leftPosPixels + (MainImGUI::getInstance()->getGUIWidth() * m_width);
        m_joystickOrigin.x = leftPosPixels + ((rightPosPixels - leftPosPixels) * 0.5f);

        float topPosPixels = m_topPos * MainImGUI::getInstance()->getGUIHeight();
        float bottomPosPixels = topPosPixels + (m_height * MainImGUI::getInstance()->getGUIHeight());
        m_joystickOrigin.y = topPosPixels + ((bottomPosPixels - topPosPixels) * 0.5f);
    }

    Joystick::~Joystick()
    {

    }

    void Joystick::updateBeforePhysics()
    {
        m_touched = false;
        m_touchedDirectionFromOrigin = glm::vec2{0.0f, 0.0f};

        std::vector<Finger>& fingers = EventHandler::getFingers();

        for(Finger& f : fingers)
        {
            if(f.normalizedPos.x > m_leftPos && f.normalizedPos.x < m_leftPos + m_width
               && f.normalizedPos.y > m_topPos && f.normalizedPos.y < m_topPos + m_height)
            {
                // If any finger in joystick area.
                m_touched = true;

                if(f.downEvent && !f.handled)
                    f.handled = true;

                m_touchedDirectionFromOrigin = glm::normalize(f.ImGuiScreenPos - m_joystickOrigin);
                m_touchedDirectionFromOrigin.y = -m_touchedDirectionFromOrigin.y;
            }
        }
    }

    void Joystick::updateAfterPhysics()
    {

    }

    void Joystick::draw()
    {
        ImGui::SetNextWindowPos(ImVec2(m_leftPos * MainImGUI::getInstance()->getGUIWidth(), m_topPos * MainImGUI::getInstance()->getGUIHeight()));
        ImGui::Begin(m_stringID.c_str(), nullptr, m_noBackgroundNoFrame);

        if(m_touched && m_touchedTexture)
        {
            ImGui::ImageButton(m_stringID.c_str(),
                               reinterpret_cast<ImTextureID>(m_touchedTexture->getID()),
                               ImVec2(m_width * MainImGUI::getInstance()->getGUIWidth(), m_height * MainImGUI::getInstance()->getGUIHeight()));
        }
        else
        {
            ImGui::ImageButton(m_stringID.c_str(),
                               reinterpret_cast<ImTextureID>(m_defaultTexture->getID()),
                               ImVec2(m_width * MainImGUI::getInstance()->getGUIWidth(), m_height * MainImGUI::getInstance()->getGUIHeight()));
        }

        ImGui::End();
    }
}
