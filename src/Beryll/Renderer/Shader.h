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

        // for uniform variables in shader
        virtual void set1Float(const char* name, const float x) = 0;
        virtual void set2Float(const char* name, const float x, const float y) = 0;
        virtual void set3Float(const char* name, const float x, const float y, const float z) = 0;
        virtual void set4Float(const char* name, const float x, const float y, const float z, const float w) = 0;

        virtual void set1Int(const char* name, const int x) = 0;
        virtual void set2Int(const char* name, const int x, const int y) = 0;
        virtual void set3Int(const char* name, const int x, const int y, const int z) = 0;
        virtual void set4Int(const char* name, const int x, const int y, const int z, const int w) = 0;

        virtual void setMatrix4x4Float(const char* name, const glm::mat4& value) = 0;
        virtual void setMatrix4x4Float(const char* name, const aiMatrix4x4& value) = 0; // for assimp matrix
        virtual void setMatrix3x3Float(const char* name, const glm::mat3& value) = 0;

        // for texture
        virtual void activateDiffuseTexture() = 0;
        virtual void activateSpecularTexture() = 0;
        virtual void activateNormalMapTexture() = 0;
        virtual void activateHeightMapTexture() = 0;
        virtual void activateSkyBoxTexture() = 0;
        virtual void activateShadowMapTexture() = 0;

    protected:
        // keep shaders in static map in subclasses for reuse them
        // key = vertexPath + fragmentPath
        // value = shader program
        // check in subclass existing shader program
    };
}