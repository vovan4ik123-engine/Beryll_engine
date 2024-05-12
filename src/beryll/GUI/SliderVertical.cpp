#include "SliderVertical.h"
#include "beryll/core/EventHandler.h"
#include "MainImGUI.h"

namespace Beryll
{
    SliderVertical::SliderVertical(const std::string& text, const std::string& fontPath, float fontHeightInPercentOfScreen,
                                   float l, float t, float w, float h, float min, float max, bool background, bool bringToFrontOnFocus)
                                   : m_text(text), m_min(min), m_max(max), m_sliderValue(min)
    {
        BR_ASSERT((fontPath.empty() == false && fontHeightInPercentOfScreen > 0.0f), "%s", "fontPath can not be empty and fontHeight must be > 0.0.");

        leftPos = l / 100.0f;
        topPos = t / 100.0f;
        width = w / 100.0f;
        height = h / 100.0f;

        if(!bringToFrontOnFocus)
        {
            m_noBackgroundNoFrame = m_noBackgroundNoFrame | ImGuiWindowFlags_NoBringToFrontOnFocus;
            m_noFrame = m_noFrame | ImGuiWindowFlags_NoBringToFrontOnFocus;
        }

        if(background)
            m_flags = m_noFrame;
        else
            m_flags = m_noBackgroundNoFrame;

        // m_text can not be empty because it is ID for slider window.
        if(m_text.empty())
            m_text = "##ImGUILibrarySpecificID" + m_IDAsString;

        m_font = MainImGUI::getInstance()->createFont(fontPath, fontHeightInPercentOfScreen);
    }

    SliderVertical::~SliderVertical()
    {

    }

    void SliderVertical::updateBeforePhysics()
    {
        m_valueChanging = false;

        std::vector<Finger>& fingers = EventHandler::getFingers();

        if(m_fingerIDDownEvent != -1)
        {
            bool touchedFingerStillOnScreen = false;
            for(Finger& f : fingers)
            {
                if(m_fingerIDDownEvent == f.ID)
                {
                    touchedFingerStillOnScreen = true;
                }
            }

            if(!touchedFingerStillOnScreen)
                m_fingerIDDownEvent = -1;
        }

        for(Finger& f : fingers)
        {
            if(f.normalizedPos.x > leftPos && f.normalizedPos.x < leftPos + width &&
               f.normalizedPos.y > topPos && f.normalizedPos.y < topPos + height)
            {
                // If any finger in slider area.
                if(f.downEvent && !f.handled)
                {
                    m_fingerIDDownEvent = f.ID;
                    f.handled = true;
                }
            }

            // Touched finger still on screen.
            if(m_fingerIDDownEvent != -1 && f.ID == m_fingerIDDownEvent)
            {
                // Calculate position on slider and set m_sliderValue here because ImGUI ignore screen multitouch.
                // Next modify ImGUI library:
                // In method bool ImGui::SliderBehaviorT() in file imgui_widgets.cpp delete if (set_new_value) {...}
                // Now we calculate slider value. Not ImGUI.
                float fingerYPos = f.normalizedPos.y;
                if(fingerYPos <= topPos)
                    fingerYPos = topPos;
                if(fingerYPos >= topPos + height)
                    fingerYPos = topPos + height;

                float normalizedSliderProgress = (fingerYPos - topPos) / height;
                if(normalizedSliderProgress < 0.0f)
                    normalizedSliderProgress = 0.0f;
                if(normalizedSliderProgress > 1.0f)
                    normalizedSliderProgress = 1.0f;

                normalizedSliderProgress = 1.0f - normalizedSliderProgress;

                float valueRange = m_max - m_min;
                m_sliderValue = m_min + (valueRange * normalizedSliderProgress);
                m_valueChanging = true;

                return;
            }
        }
    }

    void SliderVertical::updateAfterPhysics()
    {

    }

    void SliderVertical::draw()
    {
        ImGui::PushStyleColor(ImGuiCol_Text, m_fontColor);

        ImGui::PushStyleColor(ImGuiCol_SliderGrab, m_sliderGrabColor);
        ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, m_sliderGrabColor);

        ImGui::PushStyleColor(ImGuiCol_WindowBg, m_textBackGroundColor);

        ImGui::PushStyleColor(ImGuiCol_FrameBg, m_dragAreaColor);
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, m_dragAreaColor);
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, m_dragAreaColor);

        ImGui::SetNextWindowPos(ImVec2(leftPos * MainImGUI::getInstance()->getGUIWidth(), topPos * MainImGUI::getInstance()->getGUIHeight()));

        ImGui::Begin(m_IDAsString.c_str(), nullptr, m_flags);

        if(m_font)
            ImGui::PushFont(m_font);

        ImGui::VSliderFloat(m_text.c_str(),
                            ImVec2(width * MainImGUI::getInstance()->getGUIWidth(), height * MainImGUI::getInstance()->getGUIHeight()),
                            &m_sliderValue,
                            m_min,
                            m_max);

        if(m_font)
            ImGui::PopFont();

        ImGui::End();

        ImGui::PopStyleColor(7);
    }
}
