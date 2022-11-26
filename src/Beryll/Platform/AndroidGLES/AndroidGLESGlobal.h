#pragma once

// OpenGL is right-handed coordinate system in object space and world space.

// these variables should store actual bound things (current shader, texture, ...) in GLES
// check them before you want change GLES state (bind shader, texture, ...)
// default values are 0 = not bound any shader/texture
namespace GLESStateVariables
{
    // shader
    extern unsigned int currentShaderProgram;

    // Vertex Array Object
    extern unsigned int currentVAO;

    // textures ID bound to specific texture unit
    // use IDs 0 - 9 for rebinded textures
    // you can bind to these IDs as much times as you need for every object
    extern unsigned int currentTexture0; // GL_TEXTURE0
    extern unsigned int currentTexture1; // GL_TEXTURE1
    extern unsigned int currentTexture2; // GL_TEXTURE2
    //extern unsigned int currentTexture3; // GL_TEXTURE3
    //extern unsigned int currentTexture4; // GL_TEXTURE4

    // use IDs 5, 6 for booked textures. These IDs should be same all time after first binding
    // for example SkyBox and ShadowMap are same for all scene
    // and can live in their IDs all time without rebinding needed
    extern unsigned int currentTexture5; // GL_TEXTURE5 booked for SkyBox. Dont use this ID for other textures
    extern unsigned int currentTexture6; // GL_TEXTURE6 booked for ShadowMap. Dont use this ID for other textures

    // Use more than 8 textures unit can be dangerous. Not all phones support that.

    // framebuffer
}