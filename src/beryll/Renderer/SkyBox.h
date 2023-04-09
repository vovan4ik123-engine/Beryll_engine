#pragma once

#include "CppHeaders.h"
#include "Shader.h"

namespace Beryll
{
    class SkyBox
    {
    public:
        virtual ~SkyBox() {}

        virtual void draw() = 0;

        bool useInternalShader = true;
    };
}
