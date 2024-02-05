#pragma once

// OpenGL is right-handed coordinate system in object space and world space.

// These variables should store actual bound things (current shader, texture, ...) in GLES.
// Check them before you want change GLES state (bind shader, texture, ...).
// Default values are 0 = not bound any shader/texture.
namespace GLESStateVariables
{
    // Current shader.
    inline unsigned int currentShaderProgram = 0;

    // Current vertex array object.
    inline unsigned int currentVAO = 0;

    // Textures ID bound to specific texture unit (GL_TEXTURE0 = OpenGL texture unit with ID 0).

    // Material 1.
    inline unsigned int currentDiffuseTextureMat1ID0 = 0; // GL_TEXTURE0
    inline unsigned int currentSpecularTextureMat1ID1 = 0; // GL_TEXTURE1
    inline unsigned int currentNormalMapTextureMat1ID2 = 0; // GL_TEXTURE2

    // Material 2.
    inline unsigned int currentDiffuseTextureMat2ID3 = 0; // GL_TEXTURE3
    inline unsigned int currentSpecularTextureMat2ID4 = 0; // GL_TEXTURE4
    inline unsigned int currentNormalMapTextureMat2ID5 = 0; // GL_TEXTURE5
    // Only material 2 should has blending texture to blend material 2 with material 1.
    inline unsigned int currentBlendTextureMat2ID6 = 0; // GL_TEXTURE6

    // IDs 7, 8 are for booked textures.
    // SkyBox and ShadowMap are same for all scene and can live in their IDs all time without rebinding.
    inline unsigned int currentSkyBoxTextureID7 = 0; // GL_TEXTURE7 booked for SkyBox. Dont use this ID for other textures.
    inline unsigned int currentShadowMapTextureID8 = 0; // GL_TEXTURE8 booked for ShadowMap. Dont use this ID for other textures.

    // framebuffer
    // ...

    inline bool faceCullingEnabled = false;
}
