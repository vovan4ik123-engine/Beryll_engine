#pragma once

#include "beryll/core/GameObject.h"
#include "beryll/core/Window.h"

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

        float leftPos; // In range 0 = 0%...1 = 100% of screen size.
        float topPos;
        float width;
        float height;

        // New GUI vars
        glm::vec3 leftBottomPosInPercents; // X,Y in screen percents (0...100), Z in value as is (0...1).
        glm::vec3 leftBottomPosInPixels; // X,Y in screen resolution, Z in value as is (0...1).
        glm::vec3 leftBottomPosNormalized; // X,Y in 0...1 range, Z in value as is (0...1).

        glm::vec2 getLeftBottomPosInPercents()
        {
            return glm::vec2{leftBottomPosInPercents.x, leftBottomPosInPercents.y};
        }

        void setLeftBottomPosInPercents(glm::vec2 newLeftBottomPosInPercents)
        {
            BR_ASSERT((newLeftBottomPosInPercents.x >= 0.0f && newLeftBottomPosInPercents.x <= 100.0f &&
                       newLeftBottomPosInPercents.y >= 0.0f && newLeftBottomPosInPercents.y <= 100.0f), "%s", "PosInPercents must be between 0.0f and 100.0f.");

            leftBottomPosInPercents.x = newLeftBottomPosInPercents.x;
            leftBottomPosInPercents.y = newLeftBottomPosInPercents.y;

            leftBottomPosNormalized.x = leftBottomPosInPercents.x / 100.0f;
            leftBottomPosNormalized.y = leftBottomPosInPercents.y / 100.0f;

            leftBottomPosInPixels.x = leftBottomPosNormalized.x * Window::getInstance()->getScreenWidth();
            leftBottomPosInPixels.y = leftBottomPosNormalized.y * Window::getInstance()->getScreenHeight();
        }

    protected:
        // Properties only for GUI objects.
        int32_t m_noBackgroundNoFrame = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                                        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground |
                                        ImGuiWindowFlags_NoScrollbar;

        int32_t m_noFrame = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar;

        const std::string m_IDAsString = std::to_string(m_ID); // Only for GUI elements.

        bool m_isEnabled = true;
        bool m_pressed = false;
        bool m_touched = false;
    };
}
