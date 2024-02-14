#pragma once

#include "BaseAnimatedObject.h"

namespace Beryll
{
    // Animated object, not participates in physics simulation.
    class AnimatedObject : public BaseAnimatedObject
    {
    public:
        AnimatedObject() = delete;
        /*
         * filePath - path to model file (.DAE or .FBX). start path from first folder inside assets/
         * sceneGroup - game specific group to which this scene object belong
         */
        AnimatedObject(const char* filePath,
                       SceneObjectGroups sceneGroup);
        ~AnimatedObject() override;
    };
}
