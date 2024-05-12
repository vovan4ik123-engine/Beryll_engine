#pragma once

#include "LibsHeaders.h"
#include "CppHeaders.h"

#include "GUIObject.h"

namespace Beryll
{
    class Text : public GUIObject
    {
    public:
        Text() = delete;
        // left, top in percent 0...100% of screen.
        Text(const std::string& pText, const std::string& fontPath, float fontHeightInPercentOfScreen,
             float l, float t, float w, float h, bool background = false, bool bringToFrontOnFocus = false);
        ~Text() override;

        void updateBeforePhysics() override;
        void updateAfterPhysics() override;
        void draw() override;

        void setFontColor(float r, float g, float b, float a) { m_fontColor = ImVec4{ r, g, b, a }; }
        void setBackgroundColor(float r, float g, float b, float a) { m_backgroundColor = ImVec4{ r, g, b, a }; }

        std::string text;

    private:
        ImFont* m_font;
        int32_t m_flags = 0;

        // Color can be different for each text.
        ImVec4 m_fontColor { 0.0f, 0.0f, 0.0f, 1.0f }; // 0.0f - 1.0f range.
        ImVec4 m_backgroundColor { 1.0f, 1.0f, 1.0f, 1.0f };
    };
}
