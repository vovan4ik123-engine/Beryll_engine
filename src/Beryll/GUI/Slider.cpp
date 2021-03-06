#include "Slider.h"
#include "Beryll/Core/Log.h"
#include "Beryll/Core/EventHandler.h"
#include "MainImGUI.h"

namespace Beryll
{
    Slider::Slider(std::string text, float left, float top, float width, float height, bool background)
            : m_text(std::move(text)), m_leftPos(left / 100.0f), m_topPos(top / 100.0f), m_width(width / 100.0f), m_height(height / 100.0f)
    {
        BR_ASSERT((left >= 0.0f && left <= 100.0f) && (top >= 0.0f && top <= 100.0f)
                  &&(width >= 0.0f && width <= 100.0f) && (height >= 0.0f && height <= 100.0f), "Wrong slider size or position")

        if(background) { m_flags = m_backgroundNoFrame; }
        else { m_flags = m_noBackgroundNoFrame; }
    }

    Slider::~Slider()
    {

    }

    ImFont* Slider::font = nullptr;
    std::string Slider::fontPath;
    float Slider::fontHeight = 0.0f;

    void Slider::updateBeforePhysics()
    {
        std::vector<Finger>& fingers = EventHandler::getFingers();

        for(Finger& f : fingers)
        {
            if(f.normalizedPos.x > m_leftPos && f.normalizedPos.x < m_leftPos + m_width
                // add 1% of screen to slider bottom because for any reason ImGUI handle a bit more area as slider than slider has
               && f.normalizedPos.y > m_topPos && f.normalizedPos.y < m_topPos + m_height + 0.01f)
            {
                // if any finger in slider area
                if(f.downEvent && !f.handled)
                {
                    f.handled = true;
                }
            }
        }
    }

    void Slider::updateAfterPhysics()
    {

    }

    void Slider::draw()
    {
        ImGui::PushStyleColor(ImGuiCol_Text, m_fontColor);

        ImGui::PushStyleColor(ImGuiCol_SliderGrab, m_sliderGrabColor);
        ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, m_sliderGrabColor);

        ImGui::PushStyleColor(ImGuiCol_WindowBg, m_textBackGroundColor);

        ImGui::PushStyleColor(ImGuiCol_FrameBg, m_dragAreaColor);
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, m_dragAreaColor);
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, m_dragAreaColor);

        ImGui::SetNextWindowPos(ImVec2(m_leftPos * MainImGUI::getInstance()->getGUIWidth(), m_topPos * MainImGUI::getInstance()->getGUIHeight()));
        ImGui::Begin(m_stringID.c_str(), nullptr, m_flags);

        if(font)
        {
            ImGui::PushFont(font);
            ImGui::SliderFloat(m_text.c_str(),
                               ImVec2(m_width * MainImGUI::getInstance()->getGUIWidth(), m_height * MainImGUI::getInstance()->getGUIHeight()),
                               &m_sliderValue,
                               0.0f,
                               1.0f);
            ImGui::PopFont();
        }
        else
        {
            ImGui::SliderFloat(m_text.c_str(),
                               ImVec2(m_width * MainImGUI::getInstance()->getGUIWidth(), m_height * MainImGUI::getInstance()->getGUIHeight()),
                               &m_sliderValue,
                               0.0f,
                               1.0f);
        }

        ImGui::End();

        ImGui::PopStyleColor(7);
    }

    void Slider::playSound()
    {

    }
}