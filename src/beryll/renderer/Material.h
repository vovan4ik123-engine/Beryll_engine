#pragma once

#include "Texture.h"

namespace Beryll
{
    // Every loaded object must have at least one material with one diffuse texture.
    // Most objects will have this one material with one diffuse texture.
    struct Material1
    {
        void bind()
        {
            if(diffTexture) { diffTexture->bind(); }
            if(specTexture) { specTexture->bind(); }
            if(normalMapTexture) { normalMapTexture->bind(); }
        }

        std::unique_ptr<Texture> diffTexture;
        std::unique_ptr<Texture> specTexture;
        std::unique_ptr<Texture> normalMapTexture;
    };

    // Optionally object can have second material.
    // Second material must have at least diffuse texture and blend texture.
    // Blend texture is white - black and shows how to blend material1 textures with material2 textures.
    // White color on blend texture = 100% of material1 texture, black color = 100% of material2 texture,
    // gray color = 50/50.
    struct Material2
    {
        void bind()
        {
            if(diffTexture) { diffTexture->bind(); }
            if(specTexture) { specTexture->bind(); }
            if(normalMapTexture) { normalMapTexture->bind(); }
            if(blendTexture) { blendTexture->bind(); }
        }

        std::unique_ptr<Texture> diffTexture;
        std::unique_ptr<Texture> specTexture;
        std::unique_ptr<Texture> normalMapTexture;

        std::unique_ptr<Texture> blendTexture; // Or blend mask (white - black).
    };
}
