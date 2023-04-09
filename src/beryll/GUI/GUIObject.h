#pragma once

#include "beryll/core/GameObject.h"

namespace Beryll
{
    // base class of all GUI objects
    class GUIObject : public GameObject
    {
    public:
        ~GUIObject() override {}

        /*
         * inherited pure virtual methods are here
         */

        bool getIsEnabled() { return m_isEnabled; }

        void enable()
        {
            m_isEnabled = true;
        }

        void disable()
        {
            m_isEnabled = false;
        }

    protected:
        // properties only for GUI objects
        int32_t m_noBackgroundNoFrame = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                                         ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground;

        int32_t m_backgroundNoFrame = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;

        const std::string m_stringID = std::to_string(m_ID); // only for GUI elements.

        bool m_isEnabled = true;
    };
}
