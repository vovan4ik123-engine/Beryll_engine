#include "Text.h"
#include "MainImGUI.h"

namespace Beryll
{
    Text::Text(std::string text, float left, float top, bool background)
        : text(std::move(text)), m_leftPos(left / 100.0f), m_topPos(top / 100.0f)
    {
        BR_ASSERT((left >= 0 && left <= 100) && (top >= 0 && top <= 100), "%s", "Wrong Text position")

        if(background) { m_flags = m_backgroundNoFrame; }
        else { m_flags = m_noBackgroundNoFrame; }
    }

    Text::~Text()
    {

    }

    ImFont* Text::m_font = nullptr;
    std::string Text::m_fontPath;
    float Text::m_fontHeight = 0.0f;

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