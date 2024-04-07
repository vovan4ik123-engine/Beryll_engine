#pragma once

#include "GUIObject.h"

namespace Beryll
{
    class ButtonWithText : public GUIObject
    {
    public:
        ButtonWithText() = delete;
        // Position and size in percent 0...100% of screen.
        ButtonWithText(const std::string& text, const std::string& fontPath, float fontHeightInPercentOfScreen,
                       float l, float t, float w, float h, bool actRepeat = false);
        ~ButtonWithText() override;

        void updateBeforePhysics() override;
        void updateAfterPhysics() override;
        void draw() override;

        bool getIsPressed() { return m_pressed; }

        void setAction(std::function<void()> fn) { m_action = fn; }
        void setColor(float r, float g, float b, float a) { m_color = ImVec4{ r, g, b, a }; }
        void setClickedColor(float r, float g, float b, float a)  { m_clickedColor = ImVec4{ r, g, b, a }; }
        void setFontColor(float r, float g, float b, float a) { m_fontColor = ImVec4{ r, g, b, a }; }

    private:
        ImFont* m_font;

        // If action is set m_action() will called when pressed.
        std::function<void()> m_action;

        bool m_actRepeat = false; // If you want m_pressed = true all time during button touched pass actRepeat = true.

        std::string m_text;

        // Color can be different for each button.
        ImVec4 m_fontColor { 0.0f, 0.0f, 0.0f, 1.0f }; // 0.0f - 1.0f range.
        ImVec4 m_color { 0.4f, 0.4f, 0.4f, 1.0f };
        ImVec4 m_clickedColor { 0.5f, 0.5f, 0.5f, 1.0f };
    };
}
