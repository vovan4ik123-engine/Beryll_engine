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
        void setSliderGrabColor(float r, float g, float b, float a) { m_sliderGrabColor = ImVec4{ r, g, b, a }; }
        void setTextBackgroundColor(float r, float g, float b, float a) { m_textBackGroundColor = ImVec4{ r, g, b, a }; }
        void setDragAreaColor(float r, float g, float b, float a) { m_dragAreaColor = ImVec4{ r, g, b, a }; }

    private:
        friend class AndroidGLESImGUI;

        static ImFont* font; // one font for all sliders
        static std::string fontPath;
        static float fontHeight; // in range 0 = 0%...1 = 100% of screen size
        
        int32_t m_flags = 0;

        std::string m_text;
        float m_sliderValue = 0.0f; // in range 0.0f...1.0f

        float m_leftPos; // in range 0 = 0%...1 = 100% of screen size
        float m_topPos;
        float m_width;
        float m_height;

        // color can be different for each slider
        ImVec4 m_fontColor { 1.0f, 1.0f, 1.0f, 1.0f }; // 0.0f - 1.0f range
        ImVec4 m_sliderGrabColor { 0.0f, 1.0f, 0.0f, 1.0f };
        ImVec4 m_textBackGroundColor { 0.4f, 0.4f, 0.4f, 1.0f };
        ImVec4 m_dragAreaColor { 0.0f, 0.0f, 1.0f, 1.0f };
    };
}