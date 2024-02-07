#pragma once

#include "CppHeaders.h"

namespace Beryll
{
    // Forward declaration.
    class BaseSimpleObject;
    class BaseAnimatedObject;

    class ShadowMap
    {
    public:
        virtual ~ShadowMap() {}

        virtual void bind() = 0;
        virtual void unBind() = 0;
        virtual uint32_t getID() = 0; // ID in Graphics API like OpenGL ID.
        virtual int getWidth() = 0;
        virtual int getHeight() = 0;

        virtual void drawIntoShadowMap(const std::vector<std::shared_ptr<Beryll::BaseSimpleObject>>& simpleObj,
                                       const std::vector<std::shared_ptr<Beryll::BaseAnimatedObject>>& animatedObj,
                                       const glm::mat4& VPLightMatrix) = 0;
    };
}
