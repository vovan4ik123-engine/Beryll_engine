#pragma once

#include "GUIObject.h"
#include "beryll/renderer/Texture.h"

namespace Beryll
{
    class GUITexture : public GUIObject
    {
    public:
        GUITexture() = delete;
        // Position and size in 0...1 range.
        GUITexture(const std::string& texturePath, float l, float t, float w, float h, bool bringToFrontOnFocus = false);
        ~GUITexture() override;

        void updateBeforePhysics() override;
        void updateAfterPhysics() override;
        void draw() override;

    private:
        std::unique_ptr<Texture> m_texture;
    };
}
