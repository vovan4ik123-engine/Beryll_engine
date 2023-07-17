#include "AndroidGLESVertexArray.h"
#include "AndroidGLESGlobal.h"
#include "beryll/core/Log.h"

#include <GLES3/gl32.h>
#include <GLES3/gl3ext.h>

namespace Beryll
{
    AndroidGLESVertexArray::AndroidGLESVertexArray()
    {
        glGenVertexArrays(1, &m_VAO);
    }

    AndroidGLESVertexArray::~AndroidGLESVertexArray()
    {
        if(GLESStateVariables::currentVAO == m_VAO)
        {
            GLESStateVariables::currentVAO = 0;
        }

        glDeleteVertexArrays(1, &m_VAO);
    }

    void AndroidGLESVertexArray::bind()
    {
        if(GLESStateVariables::currentVAO != m_VAO)
        {
            glBindVertexArray(m_VAO);
            GLESStateVariables::currentVAO = m_VAO;
        }
    }

    void AndroidGLESVertexArray::unBind()
    {
        // Should unbind only own VAO
        if(GLESStateVariables::currentVAO == m_VAO)
        {
            glBindVertexArray(0);
            GLESStateVariables::currentVAO = 0;
        }
    }

    void AndroidGLESVertexArray::draw()
    {
        glDrawElements(GL_TRIANGLES, m_indexBuffer->getCount(), GL_UNSIGNED_INT, nullptr);
    }

    void AndroidGLESVertexArray::addVertexBuffer(const std::shared_ptr<VertexBuffer>& vertBuff)
    {
        BR_ASSERT((vertBuff->getAttribSize() != VertexAttribSize::UNKNOWN &&
                   vertBuff->getAttribType() != VertexAttribType::UNKNOWN), "%s", "Unknown vertex buffer data type or size.");

        int size = 0;
        uint32_t type = 0;
        int stride = 0;

        if(vertBuff->getAttribSize() == VertexAttribSize::ONE) { size = 1; }
        else if(vertBuff->getAttribSize() == VertexAttribSize::TWO) { size = 2; }
        else if(vertBuff->getAttribSize() == VertexAttribSize::THREE) { size = 3; }
        else if(vertBuff->getAttribSize() == VertexAttribSize::FOUR) { size = 4; }
        else if(vertBuff->getAttribSize() == VertexAttribSize::MATRIX4x4) { size = 16; }

        if(vertBuff->getAttribType() == VertexAttribType::FLOAT) { type = GL_FLOAT; stride = sizeof(float) * size; }
        else if(vertBuff->getAttribType() == VertexAttribType::INT) { type = GL_INT; stride = sizeof(int) * size; }

        bind();
        vertBuff->bind();

        if(vertBuff->getAttribType() == VertexAttribType::FLOAT &&
           vertBuff->getAttribSize() == VertexAttribSize::MATRIX4x4)
        {
            // Be carefully. Vertex shader can not accept mat4 into one layout(location = ...)
            // Max data structure accepted in one layout(location = ...) is vec4
            // So, mat4 will occupy 4 * layout(location = ...) with vec4 for every matrix column

            glEnableVertexAttribArray(m_indexNumber);
            glVertexAttribPointer(m_indexNumber, 4, type, GL_FALSE, stride, (void*)0);
            ++m_indexNumber;

            glEnableVertexAttribArray(m_indexNumber);
            glVertexAttribPointer(m_indexNumber, 4, type, GL_FALSE, stride, (void*)16);
            ++m_indexNumber;

            glEnableVertexAttribArray(m_indexNumber);
            glVertexAttribPointer(m_indexNumber, 4, type, GL_FALSE, stride, (void*)32);
            ++m_indexNumber;

            glEnableVertexAttribArray(m_indexNumber);
            glVertexAttribPointer(m_indexNumber, 4, type, GL_FALSE, stride, (void*)48);
            ++m_indexNumber;
        }
        else
        {
            glEnableVertexAttribArray(m_indexNumber);
            glVertexAttribPointer(m_indexNumber, size, type, GL_FALSE, stride, (void*)0);
            ++m_indexNumber;
        }

        vertBuff->unBind();
        unBind();

        m_vertBuffers.push_back(vertBuff);
    }

    void AndroidGLESVertexArray::setIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuff)
    {
        bind();
        indexBuff->bind();
        unBind();

        indexBuff->unBind();

        m_indexBuffer = indexBuff;
    }
}
