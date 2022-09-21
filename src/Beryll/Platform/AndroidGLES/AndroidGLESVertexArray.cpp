#include "AndroidGLESVertexArray.h"
#include "Beryll/Core/Log.h"

#include <GLES3/gl31.h>
#include <GLES3/gl3ext.h>

namespace Beryll
{
    AndroidGLESVertexArray::AndroidGLESVertexArray()
    {
        glGenVertexArrays(1, &m_VAO);
    }

    AndroidGLESVertexArray::~AndroidGLESVertexArray()
    {
        glDeleteVertexArrays(1, &m_VAO);
    }

    void AndroidGLESVertexArray::bind()
    {
        glBindVertexArray(m_VAO);
    }

    void AndroidGLESVertexArray::unBind()
    {
        glBindVertexArray(0);
    }

    void AndroidGLESVertexArray::draw()
    {
        glDrawElements(GL_TRIANGLES, m_indexBuffer->getCount(), GL_UNSIGNED_INT, 0);
    }

    void AndroidGLESVertexArray::addVertexBuffer(const std::shared_ptr<VertexBuffer>& vertBuff)
    {
        if(vertBuff->getAttribSize() == VertexAttribSize::UNKNOWN || vertBuff->getAttribType() == VertexAttribType::UNKNOWN)
        {
            BR_ASSERT(false, "%s", "Unknown vertex buffer data type or size");
        }

        uint32_t size = 0;
        uint32_t type = 0;
        uint32_t stride = 0;

        if(vertBuff->getAttribSize() == VertexAttribSize::ONE) { size = 1; }
        else if(vertBuff->getAttribSize() == VertexAttribSize::TWO) { size = 2; }
        else if(vertBuff->getAttribSize() == VertexAttribSize::THREE) { size = 3; }
        else if(vertBuff->getAttribSize() == VertexAttribSize::FOUR) { size = 4; }

        if(vertBuff->getAttribType() == VertexAttribType::FLOAT) { type = GL_FLOAT; stride = sizeof(float) * size; }
        else if(vertBuff->getAttribType() == VertexAttribType::INT) { type = GL_INT; stride = sizeof(int) * size; }


        bind();
        vertBuff->bind();

        glEnableVertexAttribArray(m_indexNumber);
        glVertexAttribPointer(m_indexNumber, size, type, GL_FALSE, stride, (void*)0);
        ++m_indexNumber;

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