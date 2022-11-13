#pragma once

#include "Beryll/GameObjects/BaseSimpleObject.h"
#include "Beryll/GameObjects/BaseAnimatedObject.h"
#include "Shader.h"

namespace Beryll
{
    enum class TextureType
    {
        UNKNOWN, // default value
        DIFFUSE_TEXTURE, // base color texture with RGB(A) colors of surface
        SPECULAR_TEXTURE, // specific texture with areas on object surface which should reflect light
        NORMAL_MAP_TEXTURE, // texture with normals per fragment
        SHADOW_MAP_TEXTURE // generated every frame from Z buffer of scene for shadows
    };

    class Texture
    {
    public:
        virtual ~Texture() {}

        virtual void bind() = 0;
        virtual void unBind() = 0;

        virtual void drawIntoShadowMap(const std::vector<std::shared_ptr<Beryll::BaseSimpleObject>>& simpleObj,
                                       const std::vector<std::shared_ptr<Beryll::BaseAnimatedObject>>& animatedObj,
                                       const glm::mat4& VPMatrix) = 0;

        TextureType getType()
        {
            return m_type;
        }

    protected:
        // keep textures in static map in subclasses for reuse them
        // key = texturePath
        // value = textureID
        // check in subclass existing texture

        TextureType m_type = TextureType::UNKNOWN;
    };
}