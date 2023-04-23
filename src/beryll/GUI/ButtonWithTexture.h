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
         * defaultTexturePath - default texture path on button. Cannot be empty
         * touchedTexturePath - texture shown when button touched. Can be empty.
         *                      If empty defaultTexturePath will shown always.
         */
        ButtonWithTexture(const char* defaultTexturePath,
                          const char* touchedTexturePath,
                          float left,
                          float top,
                          float width,
                          float height,
                          bool actRepeat = false);
        ~ButtonWithTexture() override;

        void updateBeforePhysics() override;
        void updateAfterPhysics() override;
        void draw() override;

        bool getIsPressed() { return m_pressed; }

        void setAction(std::function<void()> fn) { m_action = fn; }

    private:
        // if action is set m_action() will called when pressed
        std::function<void()> m_action;

        bool m_actRepeat = false; // if you want m_pressed = true all time during button touched pass actRepeat = true

        std::unique_ptr<Texture> m_defaultTexture;
        std::unique_ptr<Texture> m_touchedTexture;
    };
}
