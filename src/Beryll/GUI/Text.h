#pragma once

#include "LibsHeaders.h"
#include "CppHeaders.h"

#include "GUIObject.h"

namespace Beryll
{
    class Text : public GUIObject
    {
    public:
        // position in percent 0...100% of screen
        Text(std::string text, float left, float top, bool background = false);
        ~Text() override;

        void updateBeforePhysics() override;
        void updateAfterPhysics() override;
        void draw() override;
        void playSound() override;

        void setFontColor(float r, float g, float b, float a) { m_fontColor = ImVec4{ r, g, b, a }; }
        void setBackgroundColor(float r, float g, float b, float a) { m_backgroundColor = ImVec4{ r, g, b, a }; }

        static ImFont* font; // one font for all texts
        static std::string fontPath;
        static float fontHeight; // in range 0 = 0%...1 = 100% of screen size

    private:
        uint32_t m_flags = 0;

        std::string m_text;
        float m_leftPos; // in range 0 = 0%...1 = 100% of screen size
        float m_topPos;

        // collor can be different for each text
        ImVec4 m_fontColor = ImVec4{ 0.0f, 0.0f, 0.0f, 1.0f }; // 0.0f - 1.0f range
        ImVec4 m_backgroundColor = ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f };

        static uint32_t m_allTextsCount;
    };
}
