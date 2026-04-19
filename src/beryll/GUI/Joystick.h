#pragma once

#include "GUIObject.h"
#include "beryll/renderer/Texture.h"

namespace Beryll
{
    class Joystick : public GUIObject
    {
    public:
        Joystick() = delete;
        /*
         * defaultTexturePath - Cannot be empty.
         * touchedTexturePath - texture shown when touched. Can be empty.
         *                      If empty defaultTexturePath will shown always.
         */
        Joystick(const char*  defaultTexturePath,
                 const char*  touchedTexturePath,
                 const glm::vec3& pos, const glm::vec2& widthHeight, bool consumeDownEvent = true);
        ~Joystick() override;

        void updateBeforePhysics() override;
        void updateAfterPhysics() override;
        void draw() override;

        bool getIsTouched() { return m_touched; }

        // Be careful. Can be {0.0f, 0.0f}. Check length(vec2) > 0.0f.
        glm::vec2 getDirection()
        {
            return m_touchedDirectionFromOrigin;
        }

        void setOrigin(const glm::vec2 origInRange0to1);

    private:
        glm::vec2 m_originNormalized{0.0f};
        glm::vec2 m_touchedDirectionFromOrigin{0.0f};
        const glm::vec2 m_joystickUp{0.0f, 1.0f};

        std::unique_ptr<Texture> m_defaultTexture;
        std::unique_ptr<Texture> m_touchedTexture;
    };
}
