#pragma once

#include "GUIObject.h"
#include "beryll/renderer/Renderer.h"

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
         * actRepeat - if true button will considered as pressed all frames until finder up.
         */
        ButtonWithTexture(const std::string& defaultTexturePath,
                          const std::string& touchedTexturePath,
                          const glm::vec3& pos, const glm::vec2& widthHeight, bool actRepeat = false);
        ~ButtonWithTexture() override;

        void updateBeforePhysics() override;
        void updateAfterPhysics() override;
        void draw() override;

        void updateBuffersWithPositions() override;

        bool getIsPressed() { return m_pressed; }
        bool getIsPressedFingerStillOnScreen() { return m_isPressedFingerStillOnScreen; }
        bool getIsPressedFingerStillOnButton() { return m_touched; }

        void setAction(std::function<void()> fn) { m_action = fn; }

    private:
        // If action is set m_action() will called when pressed.
        std::function<void()> m_action;

        bool m_actRepeat = false; // If you want m_pressed = true all time during button touched pass actRepeat = true.

        int m_pressedFingerID = -100;
        bool m_isPressedFingerStillOnScreen = false;

        std::shared_ptr<VertexBuffer> m_vertexPosBuffer;
        std::shared_ptr<VertexBuffer> m_textureCoordsBuffer;
        std::shared_ptr<IndexBuffer> m_indexBuffer;
        std::unique_ptr<VertexArray> m_vertexArray;
        std::shared_ptr<Shader> m_internalShader;
        std::unique_ptr<Texture> m_defaultTexture;
        std::unique_ptr<Texture> m_touchedTexture;
    };
}
