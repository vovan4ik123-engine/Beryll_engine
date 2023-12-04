#include "CheckBox.h"
#include "MainImGUI.h"
#include "beryll/core/EventHandler.h"

namespace Beryll
{
    CheckBox::CheckBox(const std::string& pText, const std::string& fontPath, float fontHeightInPercentOfScreen,
                       float left, float top, bool disableCheckLogic)
                       : text(pText), m_disableCheckLogic(disableCheckLogic)
    {
        BR_ASSERT((fontPath.empty() == false && fontHeightInPercentOfScreen > 0.0f), "%s", "fontPath can not be empty and fontHeight must be > 0.0.");

        m_leftPos = left / 100.0f;
        m_topPos = top / 100.0f;
        m_width = fontHeightInPercentOfScreen / 100.0f;
        m_height = fontHeightInPercentOfScreen / 100.0f;

        if(text.empty())
            text = "##ImGUILibrarySpecificID" + m_IDAsString;

        m_font = MainImGUI::getInstance()->createFont(fontPath, fontHeightInPercentOfScreen);
    }

    CheckBox::~CheckBox()
    {

    }

    void CheckBox::updateBeforePhysics()
    {
        std::vector<Finger>& fingers = EventHandler::getFingers();
        for(Finger& f : fingers)
        {
            if(f.normalizedPos.x > m_leftPos && f.normalizedPos.x < m_leftPos + m_width &&
               f.normalizedPos.y > m_topPos && f.normalizedPos.y < m_topPos + m_height)
            {
                // If any finger in checkbox area.
                if(f.downEvent && !f.handled)
                {
                    f.handled = true;
                }
            }
        }

        if(m_valueChangingToMarked && m_action)
        {
            m_action();
        }
    }

    void CheckBox::updateAfterPhysics()
    {

    }

    void CheckBox::draw()
    {
        m_valueChangingToMarked = false;

        ImGui::PushStyleColor(ImGuiCol_Text, m_fontColor);
        ImGui::PushStyleColor(ImGuiCol_CheckMark, m_checkMarkColor);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, m_checkSquareColor);
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, m_checkSquareColor);
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, m_checkSquareColor);

        ImGui::SetNextWindowPos(ImVec2(m_leftPos * MainImGUI::getInstance()->getGUIWidth(), m_topPos * MainImGUI::getInstance()->getGUIHeight()));
        ImGui::Begin(m_IDAsString.c_str(), nullptr, m_noBackgroundNoFrame);

        if(m_font)
            ImGui::PushFont(m_font);

        if(m_disableCheckLogic)
        {
            bool avoidCheckLogic = checked;
            ImGui::Checkbox(text.c_str(), &avoidCheckLogic, true);
        }
        else
        {
            if(ImGui::Checkbox(text.c_str(), &checked, false) && checked) // True only if marked. False if unmarked.
            {
                m_valueChangingToMarked = true;
            }
        }

        if(m_font)
            ImGui::PopFont();

        ImGui::End();

        ImGui::PopStyleColor(5);
    }
}
