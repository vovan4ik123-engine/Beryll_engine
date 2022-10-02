#include "AndroidGLESShader.h"
#include "Beryll/Core/Log.h"
#include "Beryll/Utils/File.h"

#include <GLES3/gl31.h>
#include <GLES3/gl3ext.h>

namespace Beryll
{
    AndroidGLESShader::AndroidGLESShader(const char* vertexPath, const char* fragmentPath)
    {
        m_ID = vertexPath;
        m_ID += fragmentPath;

        auto result =  m_shaderPrograms.find(m_ID);
        if(result != m_shaderPrograms.end())
        {
            // shaders with given source was compiled and added before
            // use it
            //BR_INFO("%s", "shaders with given source was compiled and added before");
            m_shaderProgramID = result->second; // copy shared pointer
            return;
        }

        const char* vertexShaderCode = Utils::File::readToBuffer(vertexPath);
        const char* fragmentShaderCode = Utils::File::readToBuffer(fragmentPath);

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
            BR_ERROR("Vertex Shader failed:%s", log);
            delete[] log;
            BR_ASSERT(false, "%s", "Vertex Shader failed");
        }

        glCompileShader(fragmentShaderID);
        glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &compiled);
        if (GL_FALSE == compiled)
        {
            int len;
            glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &len);

            char* log = new char[len + 1];
            glGetShaderInfoLog(fragmentShaderID, len, &len, log); // fill log
            BR_ERROR("Fragment Shader failed:%s", log);
            delete[] log;
            BR_ASSERT(false, "%s", "Fragment Shader failed");
        }

        m_shaderProgramID = std::make_shared<uint32_t>();

        *m_shaderProgramID = glCreateProgram();
        glAttachShader(*m_shaderProgramID, vertexShaderID);
        glAttachShader(*m_shaderProgramID, fragmentShaderID);

        glLinkProgram(*m_shaderProgramID);

        glDetachShader(*m_shaderProgramID, vertexShaderID);
        glDetachShader(*m_shaderProgramID, fragmentShaderID);
        glDeleteShader(vertexShaderID);     // only mark for delete in future if was not detached !!!
        glDeleteShader(fragmentShaderID);   // Will be deleted during call glDeleteProgram(programID);
                                            // or delete now if was detached

        m_shaderPrograms.insert(std::make_pair(m_ID, m_shaderProgramID));
    }

    AndroidGLESShader::~AndroidGLESShader()
    {
        if(m_shaderProgramID.use_count() <= 2)
        {
            // use_count() <= 2 means only shared_ptr in this class left and in map
            // if we destroy this m_shaderProgramID (last copy except copy in map) also delete from OpenGL and map.
            auto result =  m_shaderPrograms.find(m_ID);
            if(result != m_shaderPrograms.end())
            {
                glDeleteProgram(*m_shaderProgramID);
                m_shaderPrograms.erase(result);
            }
        }
    }

    std::map<const std::string, std::shared_ptr<uint32_t>> AndroidGLESShader::m_shaderPrograms;

    void AndroidGLESShader::bind()
    {
        glUseProgram(*m_shaderProgramID);
    }

    void AndroidGLESShader::unBind()
    {
        glUseProgram(0);
    }

    void AndroidGLESShader::setFloat(const char* name, const float value)
    {
        glUniform1f(glGetUniformLocation(*m_shaderProgramID, name), value);
    }

    void AndroidGLESShader::setInt(const char* name, const int value)
    {
        glUniform1i(glGetUniformLocation(*m_shaderProgramID, name), value);
    }

    void AndroidGLESShader::setMatrix4x4Float(const char* name, const glm::mat4& value)
    {
        glUniformMatrix4fv(glGetUniformLocation(*m_shaderProgramID, name), 1, GL_FALSE, glm::value_ptr(value));
    }

    void AndroidGLESShader::setMatrix4x4Float(const char* name, aiMatrix4x4& value)
    {
        glUniformMatrix4fv(glGetUniformLocation(*m_shaderProgramID, name), 1, GL_TRUE, reinterpret_cast<float*>(&value));
    }

    void AndroidGLESShader::setMatrix3x3Float(const char* name, const glm::mat3& value)
    {
        glUniformMatrix3fv(glGetUniformLocation(*m_shaderProgramID, name), 1, GL_FALSE, glm::value_ptr(value));
    }

    void AndroidGLESShader::activateDiffuseTexture()
    {
        glUniform1i(glGetUniformLocation(*m_shaderProgramID, "diffuseTexture"), 0);
    }

    void AndroidGLESShader::activateSpecularTexture()
    {
        glUniform1i(glGetUniformLocation(*m_shaderProgramID, "specularTexture"), 1);
    }
}