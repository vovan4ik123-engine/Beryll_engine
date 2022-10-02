#pragma once

namespace Beryll
{
    enum class TextureType
    {
        UNKNOWN, // default value
        DIFFUSE_TEXTURE, // normal texture with RGB(A) colors of surface
        SPECULAR_TEXTURE // specific texture with areas on object surface which should reflect light
    };

    class Texture
    {
    public:
        virtual ~Texture() {}

        virtual void bind() = 0;
        virtual void unBind() = 0;

    protected:
        // keep textures in static map in subclasses for reuse them
        // key = texturePath
        // value = textureID
        // check in subclass existing texture
    };
}