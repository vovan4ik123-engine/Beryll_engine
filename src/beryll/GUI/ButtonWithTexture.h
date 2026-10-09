#pragma once

#include <utility>

#include "GUIObject.h"

namespace Beryll
{
    class ButtonWithTexture : public GUIObject
    {
    public:
        ButtonWithTexture() = delete;
        /*
         * defaultTexturePath - Cannot be empty.
         * touchedTexturePath - texture shown when touched. Can be empty. If empty defaultTexturePath will shown always.
         * pos - X,Y in screen percents (0...100), Z in value as is (0...1).
         * widthHeight - width and height in screen percents (0...100).
         * actOnTouch - if true button considered pressed on finger touch, if false on finger release.
         * actRepeat - if true button will considered as pressed all frames until finger up.
         */
        ButtonWithTexture(const char* defaultTexturePath,
                          const char* touchedTexturePath,
                          const glm::vec3& pos, const glm::vec2& widthHeight, bool actOnTouch = false, bool actRepeat = false, bool consumeDownEvent = true);
        ~ButtonWithTexture() override;

        void updateBeforePhysics() override;
        void updateAfterPhysics() override;
        void draw() override;

        bool getIsPressed() { return m_pressed; }

    private:
        bool m_actRepeat = false;

        // Vertex and index buffers are in base class.
        // ........
        std::unique_ptr<Texture> m_defaultTexture;
        std::unique_ptr<Texture> m_touchedTexture;
    };
}
