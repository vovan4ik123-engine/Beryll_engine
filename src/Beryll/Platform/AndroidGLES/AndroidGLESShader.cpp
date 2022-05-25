#include "AndroidGLESShader.h"
#include "Beryll/Core/Log.h"
#include "Beryll/Utils/File.h"

#include <GLES3/gl31.h>
#include <GLES3/gl3ext.h>

namespace Beryll
{
    AndroidGLESShader::AndroidGLESShader(const char* vertexPath, const char* fragmentPath)
    {
        std::string keyInMap = vertexPath;
        keyInMap += fragmentPath;

        auto result =  m_shaders.find(keyInMap);
        if(result != m_shaders.end())
        {
            // shaders with given source was compiled and added before
            // use it
            m_shaderProgram = result->second;
            return;
        }

        const char* vertexShaderCode = File::readToBuffer(vertexPath);
        const char* fragmentShaderCode = File::readToBuffer(fragmentPath);

        GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
        GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

        glShaderSource(vertexShaderID, 1, &vertexShaderCode, 0);
        delete[] vertexShaderCode;
        glShaderSource(fragmentShaderID, 1, &fragmentShaderCode, 0);
        delete[] fragmentShaderCode;

        glCompileShader(vertexShaderID);
        GLint compiled;
        glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &compiled);
        if (GL_FALSE == compiled)
        {
            int len;
            glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &len);

            char* log = new char[len + 1];
            glGetShaderInfoLog(vertexShaderID, len, &len, log); // fill log
            BR_ERROR("Vertex Shader failed:{0}", log);
            delete[] log;
            BR_ASSERT(false, "Vertex Shader failed");
        }

        glCompileShader(fragmentShaderID);
        glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &compiled);
        if (GL_FALSE == compiled)
        {
            int len;
            glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &len);

            char* log = new char[len + 1];
            glGetShaderInfoLog(fragmentShaderID, len, &len, log); // fill log
            BR_ERROR("Fragment Shader failed:{0}", log);
            delete[] log;
            BR_ASSERT(false, "Fragment Shader failed");
        }

        m_shaderProgram = glCreateProgram();
        glAttachShader(m_shaderProgram, vertexShaderID);
        glAttachShader(m_shaderProgram, fragmentShaderID);

        glLinkProgram(m_shaderProgram);

        glDetachShader(m_shaderProgram, vertexShaderID);
        glDetachShader(m_shaderProgram, fragmentShaderID);
        glDeleteShader(vertexShaderID);     // only mark for delete in future if was not detached !!!
        glDeleteShader(fragmentShaderID);   // Will be deleted during call glDeleteProgram(programID);
                                            // or delete now if was detached

        m_shaders.insert(std::make_pair(keyInMap, m_shaderProgram));
    }

    AndroidGLESShader::~AndroidGLESShader()
    {
        // keep shaders in static map m_shaders
        //glDeleteProgram(m_shaderProgram);
    }

    std::map<std::string, uint32_t> AndroidGLESShader::m_shaders;

    void AndroidGLESShader::bind()
    {
        glUseProgram(m_shaderProgram);
    }

    void AndroidGLESShader::unBind()
    {
        glUseProgram(0);
    }

    void AndroidGLESShader::setFloat(const char* name, const float& value)
    {
        glUniform1f(glGetUniformLocation(m_shaderProgram, name), value);
    }

    void AndroidGLESShader::setInt(const char* name, const int& value)
    {
        glUniform1i(glGetUniformLocation(m_shaderProgram, name), value);
    }

    void AndroidGLESShader::setMatrix4x4Float(const char* name, const glm::mat4& value)
    {
        glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, name), 1, GL_FALSE, glm::value_ptr(value));
    }

    void AndroidGLESShader::setMatrix4x4Float(const char* name, aiMatrix4x4& value)
    {
        glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, name), 1, GL_TRUE, reinterpret_cast<float*>(&value));
    }

    void AndroidGLESShader::setMatrix3x3Float(const char* name, const glm::mat3& value)
    {
        glUniformMatrix3fv(glGetUniformLocation(m_shaderProgram, name),
                           1, GL_FALSE, glm::value_ptr(value));
    }

    void AndroidGLESShader::activateTexture(const char* nameInShader, uint32_t index)
    {
        glUniform1i(glGetUniformLocation(m_shaderProgram, nameInShader), index);
    }
}