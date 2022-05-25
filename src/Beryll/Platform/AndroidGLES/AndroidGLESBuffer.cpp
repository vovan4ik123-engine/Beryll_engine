#include "AndroidGLESBuffer.h"

#include <GLES3/gl31.h>
#include <GLES3/gl3ext.h>

namespace Beryll
{
    // Vertex buffer
    AndroidGLESVertexBuffer::AndroidGLESVertexBuffer(const std::vector<glm::vec2>& data)
    {
        glGenBuffers(1, &m_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(glm::vec2), &data[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        m_vertAttribType = VertexAttribType::FLOAT;
        m_vertAttribSyze = VertexAttribSize::TWO;
    }

    AndroidGLESVertexBuffer::AndroidGLESVertexBuffer(const std::vector<glm::vec3>& data)
    {
        glGenBuffers(1, &m_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(glm::vec3), &data[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        m_vertAttribType = VertexAttribType::FLOAT;
        m_vertAttribSyze = VertexAttribSize::THREE;
    }

    AndroidGLESVertexBuffer::AndroidGLESVertexBuffer(const std::vector<glm::vec4>& data)
    {
        glGenBuffers(1, &m_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(glm::vec4), &data[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        m_vertAttribType = VertexAttribType::FLOAT;
        m_vertAttribSyze = VertexAttribSize::FOUR;
    }

    AndroidGLESVertexBuffer::AndroidGLESVertexBuffer(const std::vector<glm::ivec4>& data)
    {
        glGenBuffers(1, &m_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(glm::ivec4), &data[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        m_vertAttribType = VertexAttribType::INT;
        m_vertAttribSyze = VertexAttribSize::FOUR;
    }

    AndroidGLESVertexBuffer::~AndroidGLESVertexBuffer()
    {
        glDeleteBuffers(1, &m_VBO);
    }

    void AndroidGLESVertexBuffer::bind()
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    }

    void AndroidGLESVertexBuffer::unBind()
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    // Index buffer
    AndroidGLESIndexBuffer::AndroidGLESIndexBuffer(const std::vector<uint32_t>& indices)
    {
        glGenBuffers(1, &m_EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), &indices[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        m_count = indices.size();
    }

    AndroidGLESIndexBuffer::~AndroidGLESIndexBuffer()
    {
        glDeleteBuffers(1, &m_EBO);
    }

    void AndroidGLESIndexBuffer::bind()
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    }

    void AndroidGLESIndexBuffer::unBind()
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
}