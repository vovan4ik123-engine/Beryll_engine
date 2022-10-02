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

        void setFloat(const char* name, const float value) override;
        void setInt(const char* name, const int value) override;
        void setMatrix4x4Float(const char* name, const glm::mat4& value) override;
        void setMatrix4x4Float(const char* name, aiMatrix4x4& value) override;
        void setMatrix3x3Float(const char* name, const glm::mat3& value) override;

        void activateDiffuseTexture() override;
        void activateSpecularTexture() override;

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
