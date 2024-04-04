#include "ProgressBar.h"
#include "MainImGUI.h"

namespace Beryll
{
    ProgressBar::ProgressBar(float left, float top, float width, float height)
    {
        m_leftPos = left / 100.0f;
        m_topPos = top / 100.0f;
        m_width = width / 100.0f;
        m_height = height / 100.0f;
    }

    ProgressBar::~ProgressBar()
    {

    }

    void ProgressBar::updateBeforePhysics()
    {

    }

    void ProgressBar::updateAfterPhysics()
    {

    }

    void ProgressBar::draw()
    {
        ImGui::PushStyleColor(ImGuiCol_Text, m_fontColor);
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, m_progressColor);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, m_backgroundColor);

        ImGui::SetNextWindowPos(ImVec2(m_leftPos * MainImGUI::getInstance()->getGUIWidth(), m_topPos * MainImGUI::getInstance()->getGUIHeight()));
        ImGui::SetNextWindowSize(ImVec2(m_width * MainImGUI::getInstance()->getGUIWidth(), m_height * MainImGUI::getInstance()->getGUIHeight()));

        ImGui::Begin(m_IDAsString.c_str(), nullptr, m_noBackgroundNoFrame);

        ImGui::ProgressBar(m_progress);

        ImGui::End();

        ImGui::PopStyleColor(3);
    }
}
