#pragma once

#include "LibsHeaders.h"
#include "CppHeaders.h"

namespace Beryll
{
    class Shader
    {
    public:
        virtual ~Shader() {}

        virtual void bind() = 0;
        virtual void unBind() = 0;

        // for unifirm variables in shader
        virtual void setFloat(const char* name, const float value) = 0;
        virtual void setInt(const char* name, const int value) = 0;
        virtual void setMatrix4x4Float(const char* name, const glm::mat4& value) = 0;
        virtual void setMatrix4x4Float(const char* name, aiMatrix4x4& value) = 0; // for assimp matrix
        virtual void setMatrix3x3Float(const char* name, const glm::mat3& value) = 0;

        // for texture
        virtual void activateDiffuseTexture() = 0;
        virtual void activateSpecularTexture() = 0;

    protected:
        // keep shaders in static map in subclasses for reuse them
        // key = vertexPath + fragmentPath
        // value = shader program
        // check in subclass existing shader program
    };
}