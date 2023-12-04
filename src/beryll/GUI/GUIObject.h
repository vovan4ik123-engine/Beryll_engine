#pragma once

#include "beryll/core/GameObject.h"

namespace Beryll
{
    // Base class of all GUI objects.
    class GUIObject : public GameObject
    {
    public:
        ~GUIObject() override {}

        /*
         * Inherited pure virtual methods are here.
         */

        void enable() { m_isEnabled = true; }
        bool getIsEnabled() { return m_isEnabled; }

        void disable()
        {
            m_isEnabled = false;
            m_pressed = false;
            m_touched = false;
        }

    protected:
        // Properties only for GUI objects.
        int32_t m_noBackgroundNoFrame = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                                         ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground;

        int32_t m_backgroundNoFrame = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;

        const std::string m_IDAsString = std::to_string(m_ID); // Only for GUI elements.

        bool m_isEnabled = true;
        bool m_pressed = false;
        bool m_touched = false;

        float m_leftPos; // In range 0 = 0%...1 = 100% of screen size.
        float m_topPos;
        float m_width;
        float m_height;
    };
}
