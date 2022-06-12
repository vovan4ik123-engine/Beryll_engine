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

        m_ID = "Slider_" + std::to_string(m_allSliderCount);
        ++m_allSliderCount;
    }

    Slider::~Slider()
    {

    }

    uint32_t Slider::m_allSliderCount = 0;
    ImFont* Slider::font = nullptr;
    std::string Slider::fontPath;
    float Slider::fontHeight = 0.0f;

    void Slider::updateBeforePhysics()
    {
        std::vector<Finger>& fingers = EventHandler::getFingers();

        for(Finger& f : fingers)
        {
            if(f.normalizedPos.x > m_leftPos && f.normalizedPos.x < m_leftPos + m_width
               && f.normalizedPos.y > m_topPos && f.normalizedPos.y < m_topPos + m_height)
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

        ImGui::SetNextWindowPos(ImVec2(m_leftPos * MainImGUI::getInstance()->getGUIWidth(), m_topPos * MainImGUI::getInstance()->getGUIHeight()));
        ImGui::Begin(m_ID.c_str(), nullptr, m_flags);

        if(font)
        {
            ImGui::PushFont(font);
            //ImGui::SliderFloat(m_text.c_str(),
            //                    ImVec2(m_width * MainImGUI::getInstance()->getGUIWidth(), m_height * MainImGUI::getInstance()->getGUIHeight()),
            //                    &m_sliderValue, 0.0f, 1.0f);
            ImGui::PopFont();
        }
        else
        {

        }

        ImGui::End();

        ImGui::PopStyleColor(1);
    }

    void Slider::playSound()
    {

    }
}