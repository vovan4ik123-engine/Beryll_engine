#include "AndroidGLESGlobal.h"

namespace GLESStateVariables
{
    unsigned int currentShaderProgram = 0;

    unsigned int currentVAO = 0;

    unsigned int currentDiffuseTextureID0 = 0;
    unsigned int currentSpecularTextureID1 = 0;
    unsigned int currentNormalMapTextureID2 = 0;
    // 3
    // 4

    unsigned int currentSkyBoxTextureID5 = 0;
    unsigned int currentShadowMapTextureID6 = 0;

    bool faceCullingEnabled = false;
}
