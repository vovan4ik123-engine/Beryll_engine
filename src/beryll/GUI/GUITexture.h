#pragma once

#include "GUIObject.h"
#include "beryll/renderer/Texture.h"

namespace Beryll
{
    class GUITexture : public GUIObject
    {
    public:
        GUITexture() = delete;
        GUITexture(const char* texturePath,
                   const glm::vec3& pos, const glm::vec2& widthHeight);
        ~GUITexture() override;

        void updateBeforePhysics() override;
        void updateAfterPhysics() override;
        void draw() override;

    private:
        std::unique_ptr<Texture> m_texture;
    };
}
