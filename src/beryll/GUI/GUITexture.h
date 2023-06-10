#pragma once

#include "GUIObject.h"
#include "beryll/renderer/Texture.h"

namespace Beryll
{
    class GUITexture : public GUIObject
    {
    public:
        GUITexture() = delete;
        GUITexture(const std::string& texturePath, float left, float top, float width, float height);
        ~GUITexture() override;

        void updateBeforePhysics() override;
        void updateAfterPhysics() override;
        void draw() override;

    private:
        std::unique_ptr<Texture> m_texture;
    };
}
