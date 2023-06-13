#include "Text.h"
#include "MainImGUI.h"

namespace Beryll
{
    Text::Text(const std::string& t, const std::string& fontPath, float fontHeightInPercentOfScreen,
               float left, float top, bool background)
               : text(t)
    {
        BR_ASSERT((fontPath.empty() == false && fontHeightInPercentOfScreen > 0.0f), "%s", "fontPath can not be empty and fontHeight must be > 0.0.");

        m_leftPos = left / 100.0f;
        m_topPos = top / 100.0f;

        if(background)
            m_flags = m_backgroundNoFrame;
        else
            m_flags = m_noBackgroundNoFrame;

        m_font = MainImGUI::getInstance()->createFont(fontPath.c_str(), fontHeightInPercentOfScreen);
    }

    Text::~Text()
    {

    }

    void Text::updateBeforePhysics()
    {

    }

    void Text::updateAfterPhysics()
    {

    }

    void Text::draw()
    {
        ImGui::PushStyleColor(ImGuiCol_Text, m_fontColor);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, m_backgroundColor);

        ImGui::SetNextWindowPos(ImVec2(m_leftPos * MainImGUI::getInstance()->getGUIWidth(), m_topPos * MainImGUI::getInstance()->getGUIHeight()));
        ImGui::Begin(m_stringID.c_str(), nullptr, m_flags);

        if(m_font)
        {
            ImGui::PushFont(m_font);
            ImGui::Text("%s", text.c_str());
            ImGui::PopFont();
        }
        else
        {
            ImGui::Text("%s", text.c_str());
        }

        ImGui::End();

        ImGui::PopStyleColor(2);
    }
}
