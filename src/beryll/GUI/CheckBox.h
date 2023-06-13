#pragma once

#include "GUIObject.h"

namespace Beryll
{
    class CheckBox : public GUIObject
    {
    public:
        CheckBox() = delete;
        // Position in percent 0...100% of screen.
        CheckBox(const std::string& text, const std::string& fontPath, float fontHeightInPercentOfScreen,
                 float left, float top);
        ~CheckBox() override;

        void updateBeforePhysics() override;
        void updateAfterPhysics() override;
        void draw() override;

        bool getIsChecked() { return m_checked; }
        bool getIsValueChanging() { return m_valueChanging; }

        void setAction(std::function<void()> fn) { m_action = fn; }
        void setFontColor(float r, float g, float b, float a) { m_fontColor = ImVec4{ r, g, b, a }; }
        void setCheckSquareColor(float r, float g, float b, float a) { m_checkSquareColor = ImVec4{ r, g, b, a }; }
        void setCheckMarkColor(float r, float g, float b, float a) { m_checkMarkColor = ImVec4{ r, g, b, a }; }

    private:
        ImFont* m_font;

        // If action is set m_action() will called once when checked.
        std::function<void()> m_action;

        bool m_valueChanging = false;
        bool m_checked = false;

        std::string m_text;

        // Color can be different for each check box.
        ImVec4 m_fontColor { 0.0f, 0.0f, 0.0f, 1.0f }; // 0.0f - 1.0f range.
        ImVec4 m_checkMarkColor { 1.0f, 1.0f, 1.0f, 1.0f };
        ImVec4 m_checkSquareColor { 0.0f, 0.7f, 0.0f, 1.0f };
    };
}
