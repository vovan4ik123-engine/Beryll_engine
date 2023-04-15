#include "ButtonWithText.h"
#include "beryll/core/EventHandler.h"
#include "MainImGUI.h"

namespace Beryll
{
    ButtonWithText::ButtonWithText(std::string text, float left, float top, float width, float height, bool actRepeat)
        : m_text(std::move(text)), m_leftPos(left / 100.0f), m_topPos(top / 100.0f), m_width(width / 100.0f), m_height(height / 100.0f)
    {
        m_actRepeat = actRepeat;
    }

    ButtonWithText::~ButtonWithText()
    {

    }

    ImFont* ButtonWithText::m_font = nullptr;
    std::string ButtonWithText::m_fontPath;
    float ButtonWithText::m_fontHeight = 0.0f;

    void ButtonWithText::updateBeforePhysics()
    {
        std::vector<Finger>& fingers = EventHandler::getFingers();

        if(fingers.empty())
        {
            m_pressed = false;
            m_touched = false;
        }
        else
        {
            if(m_actRepeat && m_pressed) // still keep button pressed or not
            {
                m_pressed = false;
                for(Finger& f : fingers)
                {
                    if(f.normalizedPos.x > m_leftPos && f.normalizedPos.x < m_leftPos + m_width
                       && f.normalizedPos.y > m_topPos && f.normalizedPos.y < m_topPos + m_height)
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
                if(f.normalizedPos.x > m_leftPos && f.normalizedPos.x < m_leftPos + m_width
                   && f.normalizedPos.y > m_topPos && f.normalizedPos.y < m_topPos + m_height)
                {
                    // if any finger in button area
                    m_touched = true;

                    if(f.downEvent && !f.handled)
                    {
                        f.handled = true;
                        m_pressed = true;
                    }
                }
            }
        }

        if(m_pressed && m_action)
        {
            m_action();
        }
    }

    void ButtonWithText::updateAfterPhysics()
    {

    }

    void ButtonWithText::draw()
    {
        ImGui::PushStyleColor(ImGuiCol_Button, m_color); // lost focus
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, m_color); // on focus
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, m_clickedColor); // clicked
        ImGui::PushStyleColor(ImGuiCol_Text, m_fontColor);


        ImGui::SetNextWindowPos(ImVec2(m_leftPos * MainImGUI::getInstance()->getGUIWidth(), m_topPos * MainImGUI::getInstance()->getGUIHeight()));
        ImGui::Begin(m_stringID.c_str(), nullptr, m_noBackgroundNoFrame);

        if(m_font)
        {
            ImGui::PushFont(m_font);
            ImGui::Button(m_text.c_str(),
                          ImVec2(m_width * MainImGUI::getInstance()->getGUIWidth(), m_height * MainImGUI::getInstance()->getGUIHeight()),
                          ImGuiButtonFlags_PressedOnClick,
                          &m_touched);
            ImGui::PopFont();
        }
        else
        {
            ImGui::Button(m_text.c_str(),
                          ImVec2(m_width * MainImGUI::getInstance()->getGUIWidth(), m_height * MainImGUI::getInstance()->getGUIHeight()),
                          ImGuiButtonFlags_PressedOnClick,
                          &m_touched);
        }

        ImGui::End();

        ImGui::PopStyleColor(4);
    }
}
