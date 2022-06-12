#pragma once

#include "GUIObject.h"

namespace Beryll
{
    class Slider : public GUIObject
    {
    public:
        // position in percent 0...100% of screen
        Slider(std::string text, float left, float top, float width, float height, bool background = false);
        ~Slider() override;

        void updateBeforePhysics() override;
        void updateAfterPhysics() override;
        void draw() override;
        void playSound() override;

        bool getValue() { return m_sliderValue; }

        void setFontColor(float r, float g, float b, float a) { m_fontColor = ImVec4{ r, g, b, a }; }

        static ImFont* font; // one font for all sliders
        static std::string fontPath;
        static float fontHeight; // in range 0 = 0%...1 = 100% of screen size

    private:
        int32_t m_flags = 0;

        std::string m_text;
        float m_sliderValue = 0.0f; // in range 0.0f...1.0f

        float m_leftPos; // in range 0 = 0%...1 = 100% of screen size
        float m_topPos;
        float m_width;
        float m_height;

        // collor can be different for each slider
        ImVec4 m_fontColor = ImVec4{ 0.0f, 0.0f, 0.0f, 1.0f }; // 0.0f - 1.0f range

        static uint32_t m_allSliderCount;
    };
}