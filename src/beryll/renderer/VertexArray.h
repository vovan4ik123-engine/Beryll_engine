#pragma once

#include "CppHeaders.h"

#include "Buffer.h"

namespace Beryll
{
    class VertexArray
    {
    public:
        virtual ~VertexArray() {}

        virtual void bind() = 0;
        virtual void unBind() = 0;
        virtual void draw() = 0;

        virtual void addVertexBuffer(const std::shared_ptr<VertexBuffer>& vertBuff) = 0;
        virtual void setIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuff) = 0;

    protected:
        std::vector<std::shared_ptr<VertexBuffer>> m_vertBuffers;
        std::shared_ptr<IndexBuffer> m_indexBuffer;
    };
}
