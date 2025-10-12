#pragma once

#include "GUIObject.h"
#include "beryll/renderer/Texture.h"

namespace Beryll
{
    class ButtonWithAnimation : public GUIObject
    {
    public:
        ButtonWithAnimation() = delete;
        /*
         * defaultTexturePath - Cannot be empty.
         * touchedTexturePath - texture shown when touched. Can be empty.
         *                      If empty defaultTexturePath will shown always.
         * texturesSetPath    - Textures set which will plays as animation frames one after one.
         */
        // Position and size in 0...1 range.
        ButtonWithAnimation(const std::string texturesPath, const std::vector<const std::string> texturesNames,
                            const float animDurationSec, bool repeatAnimation,
                            float l, float t, float w, float h, bool actRepeat = false, bool bringToFrontOnFocus = false);
        ~ButtonWithAnimation() override;

        void enable(bool restartAnim);

        void updateBeforePhysics() override;
        void updateAfterPhysics() override;
        void draw() override;

        bool getIsPressed() { return m_pressed; }
        bool getIsPressedFingerStillOnScreen() { return m_isPressedFingerStillOnScreen; }
        bool getIsPressedFingerStillOnButton() { return m_touched; }
        bool getIsAnimationFinished() { return m_animationFinished; }

        void setAction(std::function<void()> fn) { m_action = fn; }

    private:
        // If action is set m_action() will called when pressed.
        std::function<void()> m_action;

        bool m_actRepeat = false; // If you want m_pressed = true all time during button touched pass actRepeat = true.
        // Animation data.
        std::vector<std::unique_ptr<Beryll::Texture>> m_animationFrames;
        float m_animationTotalDuration = 0.0f;
        float m_animationCurrentTime = 0.0f;
        bool m_repeatAnimation = false;
        bool m_animationFinished = false;
        int m_currentFrameIndex = 0;
        float m_timeOfOneFrame = 0.0f;
        float m_animationStartTime = 0.0f;

        int m_pressedFingerID = -100;
        bool m_isPressedFingerStillOnScreen = false;
    };
}
