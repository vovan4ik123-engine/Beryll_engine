#pragma once

#include "Beryll/Core/GameObject.h"

namespace Beryll
{
    // base class of all GUI objects
    class GUIObject : public GameObject
    {
    public:
        ~GUIObject() override {}

        // inherited pure virtual methods here

    protected:
        // add properties only for GUI objects
        int32_t m_noBackgroundNoFrame = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                                         ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground;

        int32_t m_backgroundNoFrame = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;

        std::string m_soundID;
        bool m_needPlaySound = false;

        const std::string m_stringID = std::to_string(m_ID); // only for GUI elements.
    };
}
