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

    const std::string_view animatedObjDefaultVertexPath = "shaders/GLES/default/AnimatedObject.vert";
    const std::string_view animatedObjDefaultFragmentPath = "shaders/GLES/default/AnimatedObject.frag";

    const std::string_view simpleObjShadowMapVertexPath = "shaders/GLES/shadowMap/SimpleObject.vert";
    const std::string_view simpleObjShadowMapFragmentPath = "shaders/GLES/shadowMap/SimpleObject.frag";

    const std::string_view animatedObjShadowMapVertexPath = "shaders/GLES/shadowMap/AnimatedObject.vert";
    const std::string_view animatedObjShadowMapFragmentPath = "shaders/GLES/shadowMap/AnimatedObject.frag";

    const std::string_view skyBoxVertexPath = "shaders/GLES/default/SkyBox.vert";
    const std::string_view skyBoxFragmentPath = "shaders/GLES/default/SkyBox.frag";

    const std::string_view particleVertexPath = "shaders/GLES/default/Particle.vert";
    const std::string_view particleFragmentPath = "shaders/GLES/default/Particle.frag";

    const std::string_view loadingScreenVertexPath = "shaders/GLES/default/LoadingScreen.vert";
    const std::string_view loadingScreenFragmentPath = "shaders/GLES/default/LoadingScreen.frag";

    const std::string_view bannerTwoColorsProgressVertexPath = "shaders/GLES/default/BannerTwoColorsProgress.vert";
    const std::string_view bannerTwoColorsProgressFragmentPath = "shaders/GLES/default/BannerTwoColorsProgress.frag";
#elif defined(APPLE)
    const std::string_view simpleObjDefaultVertexPath = "";
    const std::string_view simpleObjDefaultFragmentPath = "";

    const std::string_view animatedObjDefaultVertexPath = "";
    const std::string_view animatedObjDefaultFragmentPath = "";

    const std::string_view simpleObjShadowMapVertexPath = "";
    const std::string_view simpleObjShadowMapFragmentPath = "";

    const std::string_view animatedObjShadowMapVertexPath = "";
    const std::string_view animatedObjShadowMapFragmentPath = "";

    const std::string_view skyBoxVertexPath = "";
    const std::string_view skyBoxFragmentPath = "";

    const std::string_view particleVertexPath = "";
    const std::string_view particleFragmentPath = "";

    const std::string_view loadingScreenVertexPath = "";
    const std::string_view loadingScreenFragmentPath = "";
#endif
}
