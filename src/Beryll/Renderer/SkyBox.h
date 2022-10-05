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
        virtual void setShader(std::shared_ptr<Shader> shader) = 0;
    };
}
