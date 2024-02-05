#pragma once

#include "beryll/renderer/Shader.h"

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
        void set2Float(const char* name, const glm::vec2& vec) override;
        void set3Float(const char* name, const glm::vec3& vec) override;
        void set4Float(const char* name, const glm::vec4& vec) override;

        void set1Int(const char* name, const int x) override;
        void set2Int(const char* name, const int x, const int y) override;
        void set3Int(const char* name, const int x, const int y, const int z) override;
        void set4Int(const char* name, const int x, const int y, const int z, const int w) override;

        void setMatrix4x4Float(const char* name, const glm::mat4& value) override;
        void setMatrix4x4Float(const char* name, const aiMatrix4x4& value) override;
        void setMatrix3x3Float(const char* name, const glm::mat3& value) override;

        // Material 1.
        void activateDiffuseTextureMat1() override;
        void activateSpecularTextureMat1() override;
        void activateNormalMapTextureMat1() override;

        // Material 2.
        void activateDiffuseTextureMat2() override;
        void activateSpecularTextureMat2() override;
        void activateNormalMapTextureMat2() override;
        void activateBlendTextureMat2() override;

        void activateSkyBoxTexture() override;
        void activateShadowMapTexture() override;

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

        struct UniformsLocations
        {
            const std::string name;
            const int id;
        };
        std::vector<UniformsLocations> m_uniformsNameID;
    };
}
