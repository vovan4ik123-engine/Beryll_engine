#pragma once

namespace Beryll
{
    enum class TextureType
    {
        UNKNOWN, // Default value.
        // Material 1.
        DIFFUSE_TEXTURE_MAT_1, // Base color texture with RGB(A) colors of surface.
        SPECULAR_TEXTURE_MAT_1, // Specific texture with areas on object surface which should reflect light.
        NORMAL_MAP_TEXTURE_MAT_1, // Texture with normals per fragment.

        // Material 2.
        DIFFUSE_TEXTURE_MAT_2,
        SPECULAR_TEXTURE_MAT_2,
        NORMAL_MAP_TEXTURE_MAT_2,
        BLEND_TEXTURE_MAT_2, // If material 2 exist it should has blend texture for blend textures from material 1 and 2.
                             // White color on blend texture = texture from mat 1, black color = texture from mat 2. Gray color = 50/50.
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
