#include "Text.h"
#include "MainImGUI.h"
#include "Beryll/Core/Log.h"

namespace Beryll
{
    Text::Text(std::string text, float left, float top, bool background)
        : m_text(std::move(text)), m_leftPos(left / 100.0f), m_topPos(top / 100.0f)
    {
        BR_ASSERT((left >= 0 && left <= 100) && (top >= 0 && top <= 100), "Wrong Text position")

        if(background) { m_flags = m_backgroundNoFrame; }
        else { m_flags = m_noBackgroundNoFrame; }

        m_ID = "Text_" + std::to_string(m_allTextsCount);
        ++m_allTextsCount;
    }

    Text::~Text()
    {

    }

    uint32_t Text::m_allTextsCount = 0;
    ImFont* Text::font = nullptr;
    std::string Text::fontPath = std::string();
    float Text::fontHeight = 0.0f;

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
        ImGui::Begin(m_ID.c_str(), nullptr, m_flags);

        if(font)
        {
            ImGui::PushFont(font);
            ImGui::Text("%s", m_text.c_str());
            ImGui::PopFont();
        }
        else
        {
            ImGui::Text("%s", m_text.c_str());
        }

        ImGui::End();

        ImGui::PopStyleColor(2);
    }

    void Text::playSound()
    {

    }

}