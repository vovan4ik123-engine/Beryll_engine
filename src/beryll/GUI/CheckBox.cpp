#include "CheckBox.h"
#include "MainImGUI.h"

namespace Beryll
{
    CheckBox::CheckBox(const std::string& text, const std::string& fontPath, float fontHeightInPercentOfScreen,
                       float left, float top)
                       : m_text(text)
    {
        BR_ASSERT((fontPath.empty() == false && fontHeightInPercentOfScreen > 0.0f), "%s", "fontPath can not be empty and fontHeight must be > 0.0.");

        m_leftPos = left / 100.0f;
        m_topPos = top / 100.0f;

        if(m_text.empty())
            m_text = "##ImGUILibrarySpecificID" + m_IDAsString;

        m_font = MainImGUI::getInstance()->createFont(fontPath, fontHeightInPercentOfScreen);
    }

    CheckBox::~CheckBox()
    {

    }

    void CheckBox::updateBeforePhysics()
    {
        if(m_valueChanging && m_action)
        {
            m_action();
        }
    }

    void CheckBox::updateAfterPhysics()
    {

    }

    void CheckBox::draw()
    {
        m_valueChanging = false;

        ImGui::PushStyleColor(ImGuiCol_Text, m_fontColor);
        ImGui::PushStyleColor(ImGuiCol_CheckMark, m_checkMarkColor);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, m_checkSquareColor);
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, m_checkSquareColor);
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, m_checkSquareColor);

        ImGui::SetNextWindowPos(ImVec2(m_leftPos * MainImGUI::getInstance()->getGUIWidth(), m_topPos * MainImGUI::getInstance()->getGUIHeight()));
        ImGui::Begin(m_IDAsString.c_str(), nullptr, m_noBackgroundNoFrame);

        if(m_font)
        {
            ImGui::PushFont(m_font);
            if(ImGui::Checkbox(m_text.c_str(), &m_checked) && m_checked) // true only if marked. false if unmarked
            {
                m_valueChanging = true;
            }
            ImGui::PopFont();
        }
        else
        {
            if(ImGui::Checkbox(m_text.c_str(), &m_checked) && m_checked) // true only if marked. false if unmarked
            {
                m_valueChanging = true;
            }
        }

        ImGui::End();

        ImGui::PopStyleColor(5);
    }
}
