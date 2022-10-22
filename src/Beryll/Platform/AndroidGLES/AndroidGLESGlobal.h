#pragma once

// these variables should store actual bound things (current shader, texture, ...) in GLES
// check them before you want change GLES state (bind shader, texture, ...)
// default values are 0 = not bound any shader/texture
namespace GLESStateVariables
{
    // shader
    extern unsigned int currentShaderProgram;

    // textures ID bound to specific texture unit
    extern unsigned int currentTexture0; // GL_TEXTURE0
    extern unsigned int currentTexture1; // GL_TEXTURE1
    extern unsigned int currentTexture2; // GL_TEXTURE2
    extern unsigned int currentTexture3; // GL_TEXTURE3
    extern unsigned int currentTexture4; // GL_TEXTURE4

    // framebuffer
}