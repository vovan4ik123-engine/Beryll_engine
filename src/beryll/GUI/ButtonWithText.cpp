#include "ButtonWithText.h"
#include "beryll/core/EventHandler.h"
#include "MainImGUI.h"

namespace Beryll
{
    ButtonWithText::ButtonWithText(const std::string& text, const std::string& fontPath, float fontHeightInPercentOfScreen,
                                   float l, float t, float w, float h, bool actRepeat)
                                   : m_text(text)
    {
        BR_ASSERT((fontPath.empty() == false && fontHeightInPercentOfScreen > 0.0f), "%s", "fontPath can not be empty and fontHeight must be > 0.0.");

        leftPos = l / 100.0f;
        topPos = t / 100.0f;
        width = w / 100.0f;
        height = h / 100.0f;

        m_actRepeat = actRepeat;

        if(m_text.empty())
            m_text = "##ImGUILibrarySpecificID" + m_IDAsString;

        m_font = MainImGUI::getInstance()->createFont(fontPath, fontHeightInPercentOfScreen);
    }

    ButtonWithText::~ButtonWithText()
    {

    }

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
            if(m_actRepeat && m_pressed) // Still keep button pressed or not.
            {
                m_pressed = false;
                for(Finger& f : fingers)
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
                    // If any finger in button area.
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
        ImGui::PushStyleColor(ImGuiCol_Button, m_color); // Lost focus.
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, m_color); // On focus.
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, m_clickedColor); // Clicked.
        ImGui::PushStyleColor(ImGuiCol_Text, m_fontColor);

        ImGui::SetNextWindowPos(ImVec2(leftPos * MainImGUI::getInstance()->getGUIWidth(), topPos * MainImGUI::getInstance()->getGUIHeight()));
        ImGui::SetNextWindowSize(ImVec2(width * MainImGUI::getInstance()->getGUIWidth(), height * MainImGUI::getInstance()->getGUIHeight()));

        ImGui::Begin(m_IDAsString.c_str(), nullptr, m_noBackgroundNoFrame);

        if(m_font)
            ImGui::PushFont(m_font);

        ImGui::Button(m_text.c_str(),
                      ImVec2(width * MainImGUI::getInstance()->getGUIWidth(), height * MainImGUI::getInstance()->getGUIHeight()),
                      ImGuiButtonFlags_PressedOnClick,
                      &m_touched);

        if(m_font)
            ImGui::PopFont();

        ImGui::End();

        ImGui::PopStyleColor(4);
    }
}
