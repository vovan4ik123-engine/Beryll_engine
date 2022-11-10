#pragma once

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
    extern unsigned int currentTexture3; // GL_TEXTURE3
    //extern unsigned int currentTexture4; // GL_TEXTURE4
    //extern unsigned int currentTexture5; // GL_TEXTURE5
    //extern unsigned int currentTexture6; // GL_TEXTURE6
    //extern unsigned int currentTexture7; // GL_TEXTURE7
    //extern unsigned int currentTexture8; // GL_TEXTURE8
    //extern unsigned int currentTexture9; // GL_TEXTURE9

    // use IDs 10 - 19 for booked textures. These IDs should be same all time after first binding
    // for example SkyBox and ShadowMap are same for all scene
    // and can live in their IDs all time without rebinding needed
    extern unsigned int currentTexture10; // GL_TEXTURE10 booked for SkyBox. Dont use this ID for other textures
    extern unsigned int currentTexture11; // GL_TEXTURE11 booked for ShadowMap. Dont use this ID for other textures
    //extern unsigned int currentTexture12;
    //extern unsigned int currentTexture13;
    //extern unsigned int currentTexture14;
    //extern unsigned int currentTexture15;
    //extern unsigned int currentTexture16;
    //extern unsigned int currentTexture17;
    //extern unsigned int currentTexture18;
    //extern unsigned int currentTexture19;

    // framebuffer
}