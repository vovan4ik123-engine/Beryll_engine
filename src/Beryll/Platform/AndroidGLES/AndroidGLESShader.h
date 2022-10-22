#pragma once

#include "Beryll/Renderer/Shader.h"

namespace Beryll
{
    class AndroidGLESShader : public Shader
    {
    public:
        AndroidGLESShader() = delete;
        ~AndroidGLESShader() override;

        void bind() override;
        void unBind() override;

        void set1Float(const char* name, const float x) override;
        void set2Float(const char* name, const float x, const float y) override;
        void set3Float(const char* name, const float x, const float y, const float z) override;
        void set4Float(const char* name, const float x, const float y, const float z, const float w) override;

        void set1Int(const char* name, const int x) override;
        void set2Int(const char* name, const int x, const int y) override;
        void set3Int(const char* name, const int x, const int y, const int z) override;
        void set4Int(const char* name, const int x, const int y, const int z, const int w) override;

        void setMatrix4x4Float(const char* name, const glm::mat4& value) override;
        void setMatrix4x4Float(const char* name, const aiMatrix4x4& value) override;
        void setMatrix3x3Float(const char* name, const glm::mat3& value) override;

        void activateDiffuseTexture() override;
        void activateSpecularTexture() override;
        void activateShadowMapTexture() override;
        void activateNormalMapTexture() override;
        void activateHeightMapTexture() override;

    private:
        friend class Renderer;
        /*
         * vertexPath - path to vertex file in asset folder
         * fragmentPath - path to fragment file in asset folder
         */
        AndroidGLESShader(const char* vertexPath, const char* fragmentPath);

        // keep shaders in static map and reuse them
        // key = vertexPath + fragmentPath
        // value = shader program
        static std::map<const std::string, std::shared_ptr<uint32_t>> m_shaderPrograms;

        std::string m_ID; // ID in static map = vertexPath + fragmentPath
                          // if many objects load same shader, shader ID will same for all of them
        std::shared_ptr<uint32_t> m_shaderProgramID; // ID in OpenGL
                                                     // will copied across all objects with same m_ID
    };
}
