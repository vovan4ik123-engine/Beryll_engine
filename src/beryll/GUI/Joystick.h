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
        // Position and size in percent 0...100% of screen.
        Joystick(const std::string& defaultTexturePath,
                 const std::string& touchedTexturePath,
                 float left, float top, float width, float height);
        ~Joystick() override;

        void updateBeforePhysics() override;
        void updateAfterPhysics() override;
        void draw() override;

        bool getIsTouched() { return m_touched; }

        // Be careful. Can be {0.0f, 0.0f}. Check length(vec2) > 0.0f if need.
        glm::vec2 getDirection()
        {
            return m_touchedDirectionFromOrigin;
        }

        // origInRange0to1 must be in range 0...1.
        // Starts from left top screen corner.
        void setOrigin(glm::vec2 origInRange0to1);

    private:
        glm::vec2 m_joystickOriginInPixels{0.0f};
        glm::vec2 m_touchedDirectionFromOrigin{0.0f};
        const glm::vec2 m_joystickUp{0.0f, 1.0f};
        long m_fingerIDDownEvent = -1;

        std::unique_ptr<Texture> m_defaultTexture;
        std::unique_ptr<Texture> m_touchedTexture;
    };
}
