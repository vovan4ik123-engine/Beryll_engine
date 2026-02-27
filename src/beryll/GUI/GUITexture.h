#pragma once

#include "GUIObject.h"
#include "beryll/renderer/Texture.h"

namespace Beryll
{
    class GUITexture : public GUIObject
    {
    public:
        GUITexture() = delete;
        /*
         * texturePath - Cannot be empty.
         * pos - X,Y in screen percents (0...100), Z in value as is (0...1).
         * widthHeight - width and height in screen percents (0...100).
         */
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
