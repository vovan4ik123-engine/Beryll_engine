#pragma once

namespace Beryll
{
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