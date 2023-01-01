#pragma once

#include "LibsHeaders.h"

namespace BeryllConstants
{
    const glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f); // length must be 1

    // Default shaders paths
#if defined(ANDROID)
    const std::string_view simpleObjDefaultVertexPath = "shaders/GLES/default/Simple.vert";
    const std::string_view simpleObjDefaultFragmentPath = "shaders/GLES/default/Simple.frag";

    const std::string_view animatedObjDefaultVertexPath = "shaders/GLES/default/Animation.vert";
    const std::string_view animatedObjDefaultFragmentPath = "shaders/GLES/default/Animation.frag";

    const std::string_view simpleObjShadowMapVertexPath = "shaders/GLES/shadowMap/Simple.vert";
    const std::string_view simpleObjShadowMapFragmentPath = "shaders/GLES/shadowMap/Simple.frag";

    const std::string_view animatedObjShadowMapVertexPath = "shaders/GLES/shadowMap/Animation.vert";
    const std::string_view animatedObjShadowMapFragmentPath = "shaders/GLES/shadowMap/Animation.frag";

    const std::string_view skyBoxVertexPath = "shaders/GLES/default/SkyBox.vert";
    const std::string_view skyBoxFragmentPath = "shaders/GLES/default/SkyBox.frag";

    const std::string_view particleVertexPath = "shaders/GLES/default/Particle.vert";
    const std::string_view particleFragmentPath = "shaders/GLES/default/Particle.frag";

    const std::string_view loadingScreenVertexPath = "shaders/GLES/default/LoadingScreen.vert";
    const std::string_view loadingScreenFragmentPath = "shaders/GLES/default/LoadingScreen.frag";
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
