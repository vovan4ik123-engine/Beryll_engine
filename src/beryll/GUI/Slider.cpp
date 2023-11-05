#include "Slider.h"
#include "beryll/core/EventHandler.h"
#include "MainImGUI.h"

namespace Beryll
{
    Slider::Slider(const std::string& text, const std::string& fontPath, float fontHeightInPercentOfScreen,
                   float left, float top, float width, float height, float min, float max, bool background)
                   : m_text(text), m_min(min), m_max(max)
    {
        BR_ASSERT((fontPath.empty() == false && fontHeightInPercentOfScreen > 0.0f), "%s", "fontPath can not be empty and fontHeight must be > 0.0.");

        m_leftPos = left / 100.0f;
        m_topPos = top / 100.0f;
        m_width = width / 100.0f;
        m_height = height / 100.0f;

        if(background)
            m_flags = m_backgroundNoFrame;
        else
            m_flags = m_noBackgroundNoFrame;

        // m_text can not be empty because it is ID for slider window.
        if(m_text.empty())
            m_text = "##SomeID" + m_IDAsString;

        m_font = MainImGUI::getInstance()->createFont(fontPath.c_str(), fontHeightInPercentOfScreen);
    }

    Slider::~Slider()
    {

    }

    void Slider::updateBeforePhysics()
    {
        m_valueChanging = false;

        std::vector<Finger>& fingers = EventHandler::getFingers();
        for(Finger& f : fingers)
        {
            if(f.normalizedPos.x > m_leftPos && f.normalizedPos.x < m_leftPos + m_width
                // Add 1% of screen to slider bottom because for any reason ImGUI handle a bit more area as slider than slider has.
               && f.normalizedPos.y > m_topPos && f.normalizedPos.y < m_topPos + m_height + 0.01f)
            {
                // If any finger in slider area.
                if(f.downEvent && !f.handled)
                    f.handled = true;

                // Calculate position on slider and set m_sliderValue here because ImGUI ignore screen multitouch.
                // Next modify ImGUI library:
                // In method bool ImGui::SliderBehaviorT() in file imgui_widgets.cpp delete if (set_new_value) {...}
                // Now we calculate slider value. Not ImGUI.
                float valueRange = m_max - m_min;
                float normalizedSliderProgress = (f.normalizedPos.x - m_leftPos) / m_width;
                BR_ASSERT((normalizedSliderProgress >= 0.0f &&
                           normalizedSliderProgress <= 1.0f), "%s", "normalizedSliderProgress is out of 0...1 range.");
                m_sliderValue = m_min + (valueRange * normalizedSliderProgress);
                m_valueChanging = true;
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
        ImGui::Begin(m_IDAsString.c_str(), nullptr, m_flags);

        if(m_font)
        {
            ImGui::PushFont(m_font);
            ImGui::SliderFloat(m_text.c_str(),
                               ImVec2(m_width * MainImGUI::getInstance()->getGUIWidth(), m_height * MainImGUI::getInstance()->getGUIHeight()),
                               &m_sliderValue,
                               m_min,
                               m_max);
            ImGui::PopFont();
        }
        else
        {
            ImGui::SliderFloat(m_text.c_str(),
                               ImVec2(m_width * MainImGUI::getInstance()->getGUIWidth(), m_height * MainImGUI::getInstance()->getGUIHeight()),
                               &m_sliderValue,
                               m_min,
                               m_max);
        }

        ImGui::End();

        ImGui::PopStyleColor(7);
    }
}
