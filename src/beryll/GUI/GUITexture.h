#pragma once

#include "GUIObject.h"
#include "beryll/renderer/Texture.h"

namespace Beryll
{
    class GUITexture : public GUIObject
    {
    public:
        GUITexture() = delete;
        GUITexture(const char* texturePath, float left, float top, float width, float height);
        ~GUITexture() override;

        void updateBeforePhysics() override;
        void updateAfterPhysics() override;
        void draw() override;

    private:
        float m_leftPos; // in range 0 = 0%...1 = 100% of screen size
        float m_topPos;
        float m_width;
        float m_height;

        std::unique_ptr<Texture> m_texture;
    };
}
