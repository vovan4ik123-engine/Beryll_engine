#pragma once

#include "LibsHeaders.h"
#include "CppHeaders.h"

#include "GUIObject.h"

namespace Beryll
{
    class GUIText : public GUIObject
    {
    public:
        ~GUIText() override {}

        /*
         * Inherited pure virtual methods are here.
         */

        std::string text;

    protected:
        glm::vec3 m_color;
        float m_scale = 1.0f;

    private:

    };
}
