#include "Text.h"
#include "MainImGUI.h"

namespace Beryll
{
    Text::Text(const std::string& pText, const std::string& fontPath, float fontHeightInPercentOfScreen,
               float l, float t, float w, float h, bool background)
               : text(pText)
    {
        BR_ASSERT((fontPath.empty() == false && fontHeightInPercentOfScreen > 0.0f), "%s", "fontPath can not be empty and fontHeight must be > 0.0.");

        leftPos = l / 100.0f;
        topPos = t / 100.0f;
        width = w / 100.0f;
        height = h / 100.0f;

        if(background)
            m_flags = m_noFrame;
        else
            m_flags = m_noBackgroundNoFrame;

        m_font = MainImGUI::getInstance()->createFont(fontPath, fontHeightInPercentOfScreen);
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

        ImGui::SetNextWindowPos(ImVec2(leftPos * MainImGUI::getInstance()->getGUIWidth(), topPos * MainImGUI::getInstance()->getGUIHeight()));
        ImGui::SetNextWindowSize(ImVec2(width * MainImGUI::getInstance()->getGUIWidth(), height * MainImGUI::getInstance()->getGUIHeight()));

        ImGui::Begin(m_IDAsString.c_str(), nullptr, m_flags);

        if(m_font)
            ImGui::PushFont(m_font);

        ImGui::Text("%s", text.c_str());

        if(m_font)
            ImGui::PopFont();

        ImGui::End();

        ImGui::PopStyleColor(2);
    }
}
