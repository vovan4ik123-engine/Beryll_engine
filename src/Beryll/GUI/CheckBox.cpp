#include "CheckBox.h"
#include "Beryll/Core/Log.h"
#include "MainImGUI.h"

namespace Beryll
{
    CheckBox::CheckBox(std::string text, float left, float top)
        : m_text(std::move(text)), m_leftPos(left / 100.0f), m_topPos(top / 100.0f)
    {
        BR_ASSERT((left >= 0 && left <= 100) && (top >= 0 && top <= 100), "Wrong Check box position")

        m_ID = "CheckBox_" + std::to_string(m_allCheckBoxCount);
        ++m_allCheckBoxCount;
    }

    CheckBox::~CheckBox()
    {

    }

    uint32_t CheckBox::m_allCheckBoxCount = 0;
    ImFont* CheckBox::font = nullptr;
    std::string CheckBox::fontPath;
    float CheckBox::fontHeight = 0.0f;

    void CheckBox::updateBeforePhysics()
    {
        if(m_checking && m_action)
        {
            m_action();
        }
    }

    void CheckBox::updateAfterPhysics()
    {

    }

    void CheckBox::draw()
    {
        m_checking = false;

        ImGui::PushStyleColor(ImGuiCol_Text, m_fontColor);
        ImGui::PushStyleColor(ImGuiCol_CheckMark, m_checkMarkColor);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, m_checkSquareColor);
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, m_checkSquareColor);
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, m_checkSquareColor);

        ImGui::SetNextWindowPos(ImVec2(m_leftPos * MainImGUI::getInstance()->getGUIWidth(), m_topPos * MainImGUI::getInstance()->getGUIHeight()));
        ImGui::Begin(m_ID.c_str(), nullptr, m_noBackgroundNoFrame);

        if(font)
        {
            ImGui::PushFont(font);
            if(ImGui::Checkbox(m_text.c_str(), &m_checked) && m_checked) // true only if marked. false if unmarked
            {
                m_checking = true;
                m_needPlaySound = true;
            }
            ImGui::PopFont();
        }
        else
        {
            if(ImGui::Checkbox(m_text.c_str(), &m_checked) && m_checked) // true only if marked. false if unmarked
            {
                m_checking = true;
                m_needPlaySound = true;
            }
        }

        ImGui::End();

        ImGui::PopStyleColor(5);
    }

    void CheckBox::playSound()
    {
        if(!m_soundID.empty() && m_needPlaySound)
        {
            m_needPlaySound = false;
            SoundsManager::playWAV(m_soundID);
        }
    }
}

