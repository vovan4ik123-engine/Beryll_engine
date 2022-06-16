#pragma once

#include "GUIObject.h"
#include "Beryll/Core/SoundsManager.h"

namespace Beryll
{
    class CheckBox : public GUIObject
    {
    public:
        // position in percent 0...100% of screen
        CheckBox(std::string text, float left, float top);
        ~CheckBox() override;

        void updateBeforePhysics() override;
        void updateAfterPhysics() override;
        void draw() override;
        void playSound() override;

        bool getIsChecked() { return m_checked; }

        void setAction(std::function<void()> fn) { m_action = fn; }
        void setFontColor(float r, float g, float b, float a) { m_fontColor = ImVec4{ r, g, b, a }; }
        void setCheckSquareColor(float r, float g, float b, float a) { m_checkSquareColor = ImVec4{ r, g, b, a }; }
        void setCheckMarkColor(float r, float g, float b, float a) { m_checkMarkColor = ImVec4{ r, g, b, a }; }
        void setSoundWAV(const char* path) { SoundsManager::loadWAV(path); m_soundID = path; }

    private:
        friend class AndroidGLESImGUI;

        static ImFont* font; // one font for all check boxes
        static std::string fontPath;
        static float fontHeight; // in range 0 = 0%...1 = 100% of screen size

        // if action is set m_action() will called once when checked
        std::function<void()> m_action;

        bool m_checking = false;
        bool m_checked = false;

        std::string m_text;
        float m_leftPos; // in range 0 = 0%...1 = 100% of screen size
        float m_topPos;

        // color can be different for each check box
        ImVec4 m_fontColor { 0.0f, 0.0f, 0.0f, 1.0f }; // 0.0f - 1.0f range
        ImVec4 m_checkMarkColor { 1.0f, 1.0f, 1.0f, 1.0f };
        ImVec4 m_checkSquareColor { 0.0f, 0.7f, 0.0f, 1.0f };

        static uint32_t m_allCheckBoxCount;
    };
}

