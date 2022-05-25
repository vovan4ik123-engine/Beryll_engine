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

        void setFloat(const char* name, const float& value) override;
        void setInt(const char* name, const int& value) override;
        void setMatrix4x4Float(const char* name, const glm::mat4& value) override;
        void setMatrix4x4Float(const char* name, aiMatrix4x4& value) override;
        void setMatrix3x3Float(const char* name, const glm::mat3& value) override;

        void activateTexture(const char* nameInShader, uint32_t index) override;

    private:
        friend class Renderer;
        AndroidGLESShader(const char* vertexPath, const char* fragmentPath);

        // keep shaders in static map and reuse them
        // key = vertexPath + fragmentPath
        // value = shader program
        static std::map<std::string, uint32_t> m_shaders;

        uint32_t m_shaderProgram = 0;
    };
}
