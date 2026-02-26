#pragma once

#include "GUIObject.h"

namespace Beryll
{
    class ButtonWithAnimation : public GUIObject
    {
    public:
        ButtonWithAnimation() = delete;
        /*
         * texturesPath - path to folder with textures(animation frames).
         * texturesNames - textures(animation frames) names.
         * animDurationSec - duration in seconds.
         * repeatAnimation -
         * pos - X,Y in screen percents (0...100), Z in value as is (0...1).
         * widthHeight - width and height in screen percents (0...100).
         * actRepeat - if true button will considered as pressed all frames until finder up.
         */
        ButtonWithAnimation(const char* texturesPath, const std::vector<const char*> texturesNames,
                            const float animDurationSec, bool repeatAnimation,
                            const glm::vec3& pos, const glm::vec2& widthHeight, bool actRepeat = false);
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
        int m_pressedFingerID = -100;
        bool m_isPressedFingerStillOnScreen = false;

        // Vertex and index buffers are in base class.
        // ........
        // Animation data.
        std::vector<std::unique_ptr<Beryll::Texture>> m_animationFrames;
        float m_animationTotalDuration = 0.0f;
        float m_animationCurrentTime = 0.0f;
        bool m_repeatAnimation = false;
        bool m_animationFinished = false;
        int m_currentFrameIndex = 0;
        float m_timeOfOneFrame = 0.0f;
        float m_animationStartTime = 0.0f;
    };
}
