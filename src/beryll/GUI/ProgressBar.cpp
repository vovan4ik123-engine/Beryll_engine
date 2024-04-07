#include "ProgressBar.h"
#include "MainImGUI.h"

namespace Beryll
{
    ProgressBar::ProgressBar(float l, float t, float w, float h)
    {
        leftPos = l / 100.0f;
        topPos = t / 100.0f;
        width = w / 100.0f;
        height = h / 100.0f;
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

        ImGui::SetNextWindowPos(ImVec2(leftPos * MainImGUI::getInstance()->getGUIWidth(), topPos * MainImGUI::getInstance()->getGUIHeight()));
        ImGui::SetNextWindowSize(ImVec2(width * MainImGUI::getInstance()->getGUIWidth(), height * MainImGUI::getInstance()->getGUIHeight()));

        ImGui::Begin(m_IDAsString.c_str(), nullptr, m_noBackgroundNoFrame);

        ImGui::ProgressBar(m_progress);

        ImGui::End();

        ImGui::PopStyleColor(3);
    }
}
