#pragma once

#include "GUIObject.h"

namespace Beryll
{
    class Slider : public GUIObject
    {
    public:
        Slider() = delete;
        // Position in percent 0...100% of screen.
        Slider(const std::string& text, const std::string& fontPath, float fontHeightInPercentOfScreen,
               float left, float top, float width, float height, float min = 0.0f, float max = 1.0f, bool background = false);
        ~Slider() override;

        void updateBeforePhysics() override;
        void updateAfterPhysics() override;
        void draw() override;

        float getValue() { return m_sliderValue; }
        float getMin() { return m_min; }
        float getMax() { return m_max; }
        bool getIsValueChanging() { return m_valueChanging; }
        void setValue(const float val)
        {
            if(val >= m_min && val <= m_max)
            {
                m_sliderValue = val;
            }
        }

        void setFontColor(float r, float g, float b, float a) { m_fontColor = ImVec4{ r, g, b, a }; }
        void setSliderGrabColor(float r, float g, float b, float a) { m_sliderGrabColor = ImVec4{ r, g, b, a }; }
        void setTextBackgroundColor(float r, float g, float b, float a) { m_textBackGroundColor = ImVec4{ r, g, b, a }; }
        void setDragAreaColor(float r, float g, float b, float a) { m_dragAreaColor = ImVec4{ r, g, b, a }; }

    private:
        ImFont* m_font;
        int32_t m_flags = 0;

        std::string m_text;
        float m_min = 0.0f;
        float m_max = 1.0f;
        float m_sliderValue = 0.0f; // In range 0.0f...1.0f.
        bool m_valueChanging = false;
        long m_fingerIDDownEvent = -1;

        // color can be different for each slider
        ImVec4 m_fontColor { 1.0f, 1.0f, 1.0f, 1.0f }; // 0.0f - 1.0f range.
        ImVec4 m_sliderGrabColor { 0.0f, 1.0f, 0.0f, 1.0f };
        ImVec4 m_textBackGroundColor { 0.4f, 0.4f, 0.4f, 1.0f };
        ImVec4 m_dragAreaColor { 0.0f, 0.0f, 1.0f, 1.0f };
    };
}
