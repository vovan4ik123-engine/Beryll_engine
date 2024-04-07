#pragma once

#include "GUIObject.h"

namespace Beryll
{
    class CheckBox : public GUIObject
    {
    public:
        CheckBox() = delete;
        // Position in percent 0...100% of screen.
        CheckBox(const std::string& pText, const std::string& fontPath, float fontHeightInPercentOfScreen,
                 float l, float t, bool disableCheckLogic = false);
        ~CheckBox() override;

        void updateBeforePhysics() override;
        void updateAfterPhysics() override;
        void draw() override;

        bool getIsValueChangingToMarked() { return m_valueChangingToMarked; }

        void setAction(std::function<void()> fn) { m_action = fn; }
        void setFontColor(float r, float g, float b, float a) { m_fontColor = ImVec4{ r, g, b, a }; }
        void setCheckSquareColor(float r, float g, float b, float a) { m_checkSquareColor = ImVec4{ r, g, b, a }; }
        void setCheckMarkColor(float r, float g, float b, float a) { m_checkMarkColor = ImVec4{ r, g, b, a }; }

        std::string text;
        bool checked = false;

    private:
        ImFont* m_font;

        // If action is set m_action() will called once when checked.
        std::function<void()> m_action;

        bool m_valueChangingToMarked = false;
        bool m_disableCheckLogic = false; // Set to true if you want control check box state from outside (by assign checked = true/false).

        // Color can be different for each check box.
        ImVec4 m_fontColor { 0.0f, 0.0f, 0.0f, 1.0f }; // 0.0f - 1.0f range.
        ImVec4 m_checkMarkColor { 1.0f, 1.0f, 1.0f, 1.0f };
        ImVec4 m_checkSquareColor { 0.0f, 0.7f, 0.0f, 1.0f };
    };
}
