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
         * repeatAnimation - true or false.
         * pos - X,Y in screen percents (0...100), Z in value as is (0...1).
         * widthHeight - width and height in screen percents (0...100).
         * actOnTouch - if true button considered pressed on finger touch, if false on finger release.
         * actRepeat - if true button will considered as pressed all frames until finger up.
         */
        ButtonWithAnimation(const char* texturesPath, const std::vector<const char*> texturesNames,
                            const float animDurationSec, bool repeatAnimation,
                            const glm::vec3& pos, const glm::vec2& widthHeight, bool actOnTouch = false, bool actRepeat = false, bool consumeDownEvent = true);
        ~ButtonWithAnimation() override;

        void enable(bool restartAnim);

        void updateBeforePhysics() override;
        void updateAfterPhysics() override;
        void draw() override;

        bool getIsPressed() { return m_pressed; }
        bool getIsAnimationFinished() { return m_animationFinished; }

    private:
        bool m_actRepeat = false;

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
