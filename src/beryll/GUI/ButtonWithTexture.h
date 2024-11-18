#pragma once

#include "GUIObject.h"
#include "beryll/renderer/Texture.h"

namespace Beryll
{
    class ButtonWithTexture : public GUIObject
    {
    public:
        ButtonWithTexture() = delete;
        /*
         * defaultTexturePath - Cannot be empty.
         * touchedTexturePath - texture shown when touched. Can be empty.
         *                      If empty defaultTexturePath will shown always.
         */
        // Position and size in 0...1 range.
        ButtonWithTexture(const std::string& defaultTexturePath,
                          const std::string& touchedTexturePath,
                          float l, float t, float w, float h, bool actRepeat = false, bool bringToFrontOnFocus = false);
        ~ButtonWithTexture() override;

        void updateBeforePhysics() override;
        void updateAfterPhysics() override;
        void draw() override;

        bool getIsPressed() { return m_pressed; }
        bool getIsPressedFingerStillOnScreen() { return m_isPressedFingerStillOnScreen; }

        void setAction(std::function<void()> fn) { m_action = fn; }

    private:
        // If action is set m_action() will called when pressed.
        std::function<void()> m_action;

        bool m_actRepeat = false; // If you want m_pressed = true all time during button touched pass actRepeat = true.

        std::unique_ptr<Texture> m_defaultTexture;
        std::unique_ptr<Texture> m_touchedTexture;

        int m_pressedFingerID = -100;
        bool m_isPressedFingerStillOnScreen = false;
    };
}
