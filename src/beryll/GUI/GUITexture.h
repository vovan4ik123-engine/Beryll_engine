#pragma once

#include "GUIObject.h"
#include "beryll/renderer/Texture.h"

namespace Beryll
{
    class GUITexture : public GUIObject
    {
    public:
        GUITexture() = delete;
        // Position and size in percent 0...100% of screen.
        GUITexture(const std::string& texturePath, float l, float t, float w, float h);
        ~GUITexture() override;

        void updateBeforePhysics() override;
        void updateAfterPhysics() override;
        void draw() override;

    private:
        std::unique_ptr<Texture> m_texture;
    };
}
