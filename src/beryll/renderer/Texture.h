#pragma once

#include "beryll/gameObjects/BaseSimpleObject.h"
#include "beryll/gameObjects/BaseAnimatedObject.h"
#include "Shader.h"

namespace Beryll
{
    enum class TextureType
    {
        UNKNOWN, // Default value.
        DIFFUSE_TEXTURE, // Base color texture with RGB(A) colors of surface.
        SPECULAR_TEXTURE, // Specific texture with areas on object surface which should reflect light.
        NORMAL_MAP_TEXTURE, // Texture with normals per fragment.
        SHADOW_MAP_TEXTURE // Generated every frame from Z buffer of scene for shadows.
    };

    class Texture
    {
    public:
        virtual ~Texture() {}

        virtual void bind() = 0;
        virtual void unBind() = 0;
        virtual uint32_t getID() = 0; // ID in Graphics API like OpenGL ID.
        virtual int getWidth() = 0;
        virtual int getHeight() = 0;

        virtual void drawIntoShadowMap(const std::vector<std::shared_ptr<Beryll::BaseSimpleObject>>& simpleObj,
                                       const std::vector<std::shared_ptr<Beryll::BaseAnimatedObject>>& animatedObj,
                                       const glm::mat4& VPLightMatrix) = 0;

        TextureType getType()
        {
            return m_type;
        }

    protected:
        // Keep textures in static map in subclasses for reuse them.
        // Key = texturePath.
        // Value = textureID.
        // Check in subclass existing texture.

        TextureType m_type = TextureType::UNKNOWN;
    };
}
