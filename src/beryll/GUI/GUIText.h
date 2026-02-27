#pragma once

#include "LibsHeaders.h"
#include "CppHeaders.h"

#include "GUIObject.h"

namespace Beryll
{
    class GUIText : public GUIObject
    {
    public:
        GUIText() = delete;
        GUIText(const glm::vec3& pos, const glm::vec2& widthHeight) : GUIObject(pos, widthHeight) {};
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
