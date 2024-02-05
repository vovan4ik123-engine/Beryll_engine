#include "AndroidGLESShader.h"
#include "beryll/core/Log.h"
#include "beryll/utils/File.h"
#include "beryll/platform/androidGLES/AndroidGLESGlobal.h"

#include <GLES3/gl32.h>
#include <GLES3/gl3ext.h>

namespace Beryll
{
    std::map<const std::string, std::shared_ptr<uint32_t>> AndroidGLESShader::m_shaderPrograms;

    AndroidGLESShader::AndroidGLESShader(const char* vertexPath, const char* fragmentPath)
    {
        m_ID = vertexPath;
        m_ID += fragmentPath;

        auto result =  m_shaderPrograms.find(m_ID);
        if(result != m_shaderPrograms.end())
        {
            // Shaders with given source was compiled and added before.
            // Use it.
            //BR_INFO("%s", "shaders with given source was compiled and added before");
            m_shaderProgramID = result->second; // Copy shared pointer.
            return;
        }

        const char* vertexShaderCode = BeryllUtils::File::readToBuffer(vertexPath);
        const char* fragmentShaderCode = BeryllUtils::File::readToBuffer(fragmentPath);

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
            BR_ERROR("Vertex Shader failed: %s", log);
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
            BR_ERROR("Fragment Shader failed: %s", log);
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
            // use_count() <= 2 means only shared_ptr in this class left and in map.
            // If we destroy this m_shaderProgramID (last copy except copy in map) also delete from OpenGL and map.
            auto result =  m_shaderPrograms.find(m_ID);
            if(result != m_shaderPrograms.end())
            {
                if(GLESStateVariables::currentShaderProgram == *m_shaderProgramID)
                {
                    GLESStateVariables::currentShaderProgram = 0;
                }

                glDeleteProgram(*m_shaderProgramID);
                m_shaderPrograms.erase(result);
            }
        }
    }

    void AndroidGLESShader::bind()
    {
        if(GLESStateVariables::currentShaderProgram != *m_shaderProgramID)
        {
            //BR_INFO("%s", "bind shader");
            glUseProgram(*m_shaderProgramID);
            GLESStateVariables::currentShaderProgram = *m_shaderProgramID;
        }
    }

    void AndroidGLESShader::unBind()
    {
        // This object can unbind only his own shader program.
        if(GLESStateVariables::currentShaderProgram == *m_shaderProgramID)
        {
            glUseProgram(0);
            GLESStateVariables::currentShaderProgram = 0;
        }
    }

    void AndroidGLESShader::set1Float(const char* name, const float x)
    {
        for(const UniformsLocations& uniforms : m_uniformsNameID)
        {
            if(uniforms.name == name)
            {
                glUniform1f(uniforms.id, x);
                return;
            }
        }
        
        int id = glGetUniformLocation(*m_shaderProgramID, name);
        glUniform1f(id, x);
        m_uniformsNameID.emplace_back(UniformsLocations{name, id});
    }

    void AndroidGLESShader::set2Float(const char* name, const glm::vec2& vec)
    {
        for(const UniformsLocations& uniforms : m_uniformsNameID)
        {
            if(uniforms.name == name)
            {
                glUniform2f(uniforms.id, vec.x, vec.y);
                return;
            }
        }
        
        int id = glGetUniformLocation(*m_shaderProgramID, name);
        glUniform2f(id, vec.x, vec.y);
        m_uniformsNameID.emplace_back(UniformsLocations{name, id});
    }

    void AndroidGLESShader::set3Float(const char* name, const glm::vec3& vec)
    {
        for(const UniformsLocations& uniforms : m_uniformsNameID)
        {
            if(uniforms.name == name)
            {
                glUniform3f(uniforms.id, vec.x, vec.y, vec.z);
                return;
            }
        }
        
        int id = glGetUniformLocation(*m_shaderProgramID, name);
        glUniform3f(id, vec.x, vec.y, vec.z);
        m_uniformsNameID.emplace_back(UniformsLocations{name, id});
    }

    void AndroidGLESShader::set4Float(const char* name, const glm::vec4& vec)
    {
        for(const UniformsLocations& uniforms : m_uniformsNameID)
        {
            if(uniforms.name == name)
            {
                glUniform4f(uniforms.id, vec.x, vec.y,vec.z,vec.w);
                return;
            }
        }
        
        int id = glGetUniformLocation(*m_shaderProgramID, name);
        glUniform4f(id, vec.x, vec.y, vec.z, vec.w);
        m_uniformsNameID.emplace_back(UniformsLocations{name, id});
    }

    void AndroidGLESShader::set1Int(const char* name, const int x)
    {
        for(const UniformsLocations& uniforms : m_uniformsNameID)
        {
            if(uniforms.name == name)
            {
                glUniform1i(uniforms.id, x);
                return;
            }
        }
        
        int id = glGetUniformLocation(*m_shaderProgramID, name);
        glUniform1i(id, x);
        m_uniformsNameID.emplace_back(UniformsLocations{name, id});
    }

    void AndroidGLESShader::set2Int(const char* name, const int x, const int y)
    {
        for(const UniformsLocations& uniforms : m_uniformsNameID)
        {
            if(uniforms.name == name)
            {
                glUniform2i(uniforms.id, x, y);
                return;
            }
        }
        
        int id = glGetUniformLocation(*m_shaderProgramID, name);
        glUniform2i(id, x, y);
        m_uniformsNameID.emplace_back(UniformsLocations{name, id});
    }

    void AndroidGLESShader::set3Int(const char* name, const int x, const int y, const int z)
    {
        for(const UniformsLocations& uniforms : m_uniformsNameID)
        {
            if(uniforms.name == name)
            {
                glUniform3i(uniforms.id, x, y, z);
                return;
            }
        }

        int id = glGetUniformLocation(*m_shaderProgramID, name);
        glUniform3i(id, x, y, z);
        m_uniformsNameID.emplace_back(UniformsLocations{name, id});
    }

    void AndroidGLESShader::set4Int(const char* name, const int x, const int y, const int z, const int w)
    {
        for(const UniformsLocations& uniforms : m_uniformsNameID)
        {
            if(uniforms.name == name)
            {
                glUniform4i(uniforms.id, x, y, z, w);
                return;
            }
        }

        int id = glGetUniformLocation(*m_shaderProgramID, name);
        glUniform4i(id, x, y, z, w);
        m_uniformsNameID.emplace_back(UniformsLocations{name, id});
    }

    void AndroidGLESShader::setMatrix4x4Float(const char* name, const glm::mat4& value)
    {
        for(const UniformsLocations& uniforms : m_uniformsNameID)
        {
            if(uniforms.name == name)
            {
                glUniformMatrix4fv(uniforms.id, 1, GL_FALSE, glm::value_ptr(value));
                return;
            }
        }

        int id = glGetUniformLocation(*m_shaderProgramID, name);
        glUniformMatrix4fv(id, 1, GL_FALSE, glm::value_ptr(value));
        m_uniformsNameID.emplace_back(UniformsLocations{name, id});
    }

    void AndroidGLESShader::setMatrix4x4Float(const char* name, const aiMatrix4x4& value)
    {
        for(const UniformsLocations& uniforms : m_uniformsNameID)
        {
            if(uniforms.name == name)
            {
                glUniformMatrix4fv(uniforms.id, 1, GL_TRUE,
                                   reinterpret_cast<float*>(const_cast<aiMatrix4x4*>(&value)));
                return;
            }
        }

        int id = glGetUniformLocation(*m_shaderProgramID, name);
        glUniformMatrix4fv(id, 1, GL_TRUE,
                           reinterpret_cast<float*>(const_cast<aiMatrix4x4*>(&value)));
        m_uniformsNameID.emplace_back(UniformsLocations{name, id});
    }

    void AndroidGLESShader::setMatrix3x3Float(const char* name, const glm::mat3& value)
    {
        for(const UniformsLocations& uniforms : m_uniformsNameID)
        {
            if(uniforms.name == name)
            {
                glUniformMatrix3fv(uniforms.id, 1, GL_FALSE, glm::value_ptr(value));
                return;
            }
        }

        int id = glGetUniformLocation(*m_shaderProgramID, name);
        glUniformMatrix3fv(id, 1, GL_FALSE, glm::value_ptr(value));
        m_uniformsNameID.emplace_back(UniformsLocations{name, id});
    }

    void AndroidGLESShader::activateDiffuseTextureMat1()
    {
        glUniform1i(glGetUniformLocation(*m_shaderProgramID, "diffuseTexture"), 0);
    }

    void AndroidGLESShader::activateSpecularTextureMat1()
    {
        glUniform1i(glGetUniformLocation(*m_shaderProgramID, "specularTexture"), 1);
    }

    void AndroidGLESShader::activateNormalMapTextureMat1()
    {
        glUniform1i(glGetUniformLocation(*m_shaderProgramID, "normalMapTexture"), 2);
    }

    void AndroidGLESShader::activateDiffuseTextureMat2()
    {
        glUniform1i(glGetUniformLocation(*m_shaderProgramID, "diffuseTextureMat2"), 3);
    }

    void AndroidGLESShader::activateSpecularTextureMat2()
    {
        glUniform1i(glGetUniformLocation(*m_shaderProgramID, "specularTextureMat2"), 4);
    }

    void AndroidGLESShader::activateNormalMapTextureMat2()
    {
        glUniform1i(glGetUniformLocation(*m_shaderProgramID, "normalMapTextureMat2"), 5);
    }

    void AndroidGLESShader::activateBlendTextureMat2()
    {
        glUniform1i(glGetUniformLocation(*m_shaderProgramID, "blendTextureMat2"), 6);
    }

    void AndroidGLESShader::activateSkyBoxTexture()
    {
        glUniform1i(glGetUniformLocation(*m_shaderProgramID, "skyBoxTexture"), 7);
    }

    void AndroidGLESShader::activateShadowMapTexture()
    {
        glUniform1i(glGetUniformLocation(*m_shaderProgramID, "shadowMapTexture"), 8);
    }
}
