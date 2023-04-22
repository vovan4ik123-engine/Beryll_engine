#pragma once

#include "GUIObject.h"
#include "beryll/renderer/Texture.h"

namespace Beryll
{
    class Joystick : public GUIObject
    {
    public:
        Joystick() = delete;
        Joystick(const char* defaultTexturePath, const char* touchedTexturePath, float left, float top, float width, float height);
        ~Joystick() override;

        void updateBeforePhysics() override;
        void updateAfterPhysics() override;
        void draw() override;

        bool getIsTouched() { return m_touched; }
        glm::vec2 getDirection()
        {
            BR_ASSERT(m_touched, "%s", "Joystick not touched. Call getIsTouched() to check.");
            return m_touchedDirectionFromOrigin;
        }

    private:
        bool m_touched = false;

        float m_leftPos; // in range 0 = 0%...1 = 100% of screen size
        float m_topPos;
        float m_width;
        float m_height;
        glm::vec2 m_joystickOrigin{0.0f};
        glm::vec2 m_touchedDirectionFromOrigin{0.0f};
        const glm::vec2 m_joystickUp{0.0f, 1.0f};

        std::unique_ptr<Texture> m_defaultTexture;
        std::unique_ptr<Texture> m_touchedTexture;
    };
}
