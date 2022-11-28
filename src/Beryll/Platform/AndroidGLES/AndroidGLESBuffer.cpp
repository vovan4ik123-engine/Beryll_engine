#include "AndroidGLESBuffer.h"

#include <GLES3/gl31.h>
#include <GLES3/gl3ext.h>

namespace Beryll
{
    // Vertex buffer
    AndroidGLESStaticVertexBuffer::AndroidGLESStaticVertexBuffer(const std::vector<glm::vec2>& data)
    {
        glGenBuffers(1, &m_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(glm::vec2), data.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        m_vertAttribType = VertexAttribType::FLOAT;
        m_vertAttribSyze = VertexAttribSize::TWO;
    }

    AndroidGLESStaticVertexBuffer::AndroidGLESStaticVertexBuffer(const std::vector<glm::vec3>& data)
    {
        glGenBuffers(1, &m_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(glm::vec3), data.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        m_vertAttribType = VertexAttribType::FLOAT;
        m_vertAttribSyze = VertexAttribSize::THREE;
    }

    AndroidGLESStaticVertexBuffer::AndroidGLESStaticVertexBuffer(const std::vector<glm::vec4>& data)
    {
        glGenBuffers(1, &m_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(glm::vec4), data.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        m_vertAttribType = VertexAttribType::FLOAT;
        m_vertAttribSyze = VertexAttribSize::FOUR;
    }

    AndroidGLESStaticVertexBuffer::AndroidGLESStaticVertexBuffer(const std::vector<glm::ivec4>& data)
    {
        glGenBuffers(1, &m_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(glm::ivec4), data.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        m_vertAttribType = VertexAttribType::INT;
        m_vertAttribSyze = VertexAttribSize::FOUR;
    }

    AndroidGLESStaticVertexBuffer::~AndroidGLESStaticVertexBuffer()
    {
        glDeleteBuffers(1, &m_VBO);
    }

    void AndroidGLESStaticVertexBuffer::bind()
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    }

    void AndroidGLESStaticVertexBuffer::unBind()
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    AndroidGLESDynamicVertexBuffer::AndroidGLESDynamicVertexBuffer(VertexAttribType type, VertexAttribSize size, uint32_t maxSizeBytes)
        : m_originalSizeBytes(maxSizeBytes)
    {
        glGenBuffers(1, &m_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, maxSizeBytes, nullptr, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        m_vertAttribType = type;
        m_vertAttribSyze = size;
    }

    AndroidGLESDynamicVertexBuffer::~AndroidGLESDynamicVertexBuffer()
    {
        glDeleteBuffers(1, &m_VBO);
    }

    void AndroidGLESDynamicVertexBuffer::setDynamicBufferData(const std::vector<glm::vec3>& data, uint32_t elementsCount)
    {
        BR_ASSERT((m_vertAttribType == VertexAttribType::FLOAT && m_vertAttribSyze == VertexAttribSize::THREE),
                  "%s", "Dynamic buffer was created with different data type or size");

        BR_ASSERT((elementsCount * sizeof(glm::vec3) <= m_originalSizeBytes),
                  "%s", "You copy more data than buffer can store");

        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, elementsCount * sizeof(glm::vec3), data.data());
    }

    void AndroidGLESDynamicVertexBuffer::setDynamicBufferData(const std::vector<glm::vec4>& data, uint32_t elementsCount)
    {
        BR_ASSERT((m_vertAttribType == VertexAttribType::FLOAT && m_vertAttribSyze == VertexAttribSize::FOUR),
                  "%s", "Dynamic buffer was created with different data type or size");

        BR_ASSERT((elementsCount * sizeof(glm::vec4) <= m_originalSizeBytes),
                  "%s", "You copy more data than buffer can store");

        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, elementsCount * sizeof(glm::vec4), data.data());
    }

    void AndroidGLESDynamicVertexBuffer::bind()
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    }

    void AndroidGLESDynamicVertexBuffer::unBind()
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    // Index buffer
    AndroidGLESStaticIndexBuffer::AndroidGLESStaticIndexBuffer(const std::vector<uint32_t>& indices) : m_originalCount(indices.size())
    {
        glGenBuffers(1, &m_EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        m_count = indices.size();
    }

    AndroidGLESStaticIndexBuffer::~AndroidGLESStaticIndexBuffer()
    {
        glDeleteBuffers(1, &m_EBO);
    }

    void AndroidGLESStaticIndexBuffer::bind()
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    }

    void AndroidGLESStaticIndexBuffer::unBind()
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void AndroidGLESStaticIndexBuffer::setCount(uint32_t count)
    {
        BR_ASSERT((count <= m_originalCount), "%s", "New count can not be more that original size");

        m_count = count;
    }
}