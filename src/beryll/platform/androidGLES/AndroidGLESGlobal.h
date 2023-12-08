#pragma once

// OpenGL is right-handed coordinate system in object space and world space.

// these variables should store actual bound things (current shader, texture, ...) in GLES
// check them before you want change GLES state (bind shader, texture, ...)
// default values are 0 = not bound any shader/texture
namespace GLESStateVariables
{
    // Current shader.
    inline unsigned int currentShaderProgram = 0;

    // Current vertex array object.
    inline unsigned int currentVAO = 0;

    // Textures ID bound to specific texture unit.
    // Texture units [0 - 4] are for object's material (diffuse, specular, normal map).
    inline unsigned int currentDiffuseTextureID0 = 0; // GL_TEXTURE0
    inline unsigned int currentSpecularTextureID1 = 0; // GL_TEXTURE1
    inline unsigned int currentNormalMapTextureID2 = 0; // GL_TEXTURE2
    //inline unsigned int currentTexture3; // GL_TEXTURE3 - free
    //inline unsigned int currentTexture4; // GL_TEXTURE4 - free

    // IDs 5, 6 are for booked textures. These IDs should be same all time after first binding.
    // For example SkyBox and ShadowMap are same for all scene and can live in their IDs all time without rebinding.
    inline unsigned int currentSkyBoxTextureID5 = 0; // GL_TEXTURE5 booked for SkyBox. Dont use this ID for other textures.
    inline unsigned int currentShadowMapTextureID6 = 0; // GL_TEXTURE6 booked for ShadowMap. Dont use this ID for other textures.

    // Use more than 8 texture units (0-7 index) can be dangerous. Not all phones support that.
    // Also for some phones 4k is max supported texture resolution.

    // framebuffer
    // ...

    inline bool faceCullingEnabled = false;
}
