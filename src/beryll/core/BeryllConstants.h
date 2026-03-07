#pragma once

#include "LibsHeaders.h"
#include "CppHeaders.h"

namespace BeryllConstants
{
    const glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f); // Length must be 1.

    // Default shaders paths
#if defined(ANDROID)
    const std::string_view simpleObjDefaultVertexPath = "shaders/GLES/default/SimpleObject.vert";
    const std::string_view simpleObjDefaultFragmentPath = "shaders/GLES/default/SimpleObject.frag";
    const std::string_view simpleObjTwoMaterialsDefaultVertexPath = "shaders/GLES/default/SimpleObjectTwoMaterials.vert";
    const std::string_view simpleObjTwoMaterialsDefaultFragmentPath = "shaders/GLES/default/SimpleObjectTwoMaterials.frag";

    const std::string_view animatedObjDefaultVertexPath = "shaders/GLES/default/AnimatedObject.vert";
    const std::string_view animatedObjDefaultFragmentPath = "shaders/GLES/default/AnimatedObject.frag";
    const std::string_view animatedObjTwoMaterialsDefaultVertexPath = "shaders/GLES/default/AnimatedObjectTwoMaterials.vert";
    const std::string_view animatedObjTwoMaterialsDefaultFragmentPath = "shaders/GLES/default/AnimatedObjectTwoMaterials.frag";

    const std::string_view simpleObjShadowMapVertexPath = "shaders/GLES/shadowMap/SimpleObject.vert";
    const std::string_view simpleObjShadowMapFragmentPath = "shaders/GLES/shadowMap/SimpleObject.frag";

    const std::string_view animatedObjShadowMapVertexPath = "shaders/GLES/shadowMap/AnimatedObject.vert";
    const std::string_view animatedObjShadowMapFragmentPath = "shaders/GLES/shadowMap/AnimatedObject.frag";

    const std::string_view skyBoxVertexPath = "shaders/GLES/default/SkyBox.vert";
    const std::string_view skyBoxFragmentPath = "shaders/GLES/default/SkyBox.frag";

    const std::string_view particleVertexPath = "shaders/GLES/default/Particle.vert";
    const std::string_view particleFragmentPath = "shaders/GLES/default/Particle.frag";

    const std::string_view GUITextVertexPath = "shaders/GLES/GUI/GUIText.vert";
    const std::string_view GUITextFragmentPath = "shaders/GLES/GUI/GUIText.frag";

    const std::string_view GUIElementWithTextureVertexPath = "shaders/GLES/GUI/GUIElementWithTexture.vert";
    const std::string_view GUIElementWithTextureFragmentPath = "shaders/GLES/GUI/GUIElementWithTexture.frag";

    const std::string_view GUIProgressBarVertexPath = "shaders/GLES/GUI/GUIProgressBar.vert";
    const std::string_view GUIProgressBarFragmentPath = "shaders/GLES/GUI/GUIProgressBar.frag";

    const std::string_view bannerTwoColorsProgressVertexPath = "shaders/GLES/default/BannerTwoColorsProgress.vert";
    const std::string_view bannerTwoColorsProgressFragmentPath = "shaders/GLES/default/BannerTwoColorsProgress.frag";

    const std::string_view textOnSceneVertexPath = "shaders/GLES/default/TextOnScene.vert";
    const std::string_view textOnSceneFragmentPath = "shaders/GLES/default/TextOnScene.frag";
#elif defined(APPLE)

#endif
}
