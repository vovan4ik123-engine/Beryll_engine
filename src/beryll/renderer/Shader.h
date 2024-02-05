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

        // For uniform variables in shader.
        virtual void set1Float(const char* name, const float x) = 0;
        virtual void set2Float(const char* name, const glm::vec2& vec) = 0;
        virtual void set3Float(const char* name, const glm::vec3& vec) = 0;
        virtual void set4Float(const char* name, const glm::vec4& vec) = 0;

        virtual void set1Int(const char* name, const int x) = 0;
        virtual void set2Int(const char* name, const int x, const int y) = 0;
        virtual void set3Int(const char* name, const int x, const int y, const int z) = 0;
        virtual void set4Int(const char* name, const int x, const int y, const int z, const int w) = 0;

        virtual void setMatrix4x4Float(const char* name, const glm::mat4& value) = 0;
        virtual void setMatrix4x4Float(const char* name, const aiMatrix4x4& value) = 0; // for assimp matrix
        virtual void setMatrix3x3Float(const char* name, const glm::mat3& value) = 0;

        // Material 1.
        virtual void activateDiffuseTextureMat1() = 0;
        virtual void activateSpecularTextureMat1() = 0;
        virtual void activateNormalMapTextureMat1() = 0;

        // Material 2.
        virtual void activateDiffuseTextureMat2() = 0;
        virtual void activateSpecularTextureMat2() = 0;
        virtual void activateNormalMapTextureMat2() = 0;
        virtual void activateBlendTextureMat2() = 0;

        virtual void activateSkyBoxTexture() = 0;
        virtual void activateShadowMapTexture() = 0;

    protected:
        // keep shaders in static map in subclasses for reuse them
        // key = vertexPath + fragmentPath
        // value = shader program
        // check in subclass existing shader program
    };
}
