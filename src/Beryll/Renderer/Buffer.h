#pragma once

namespace Beryll
{
    enum class VertexAttribSize
    {
        UNKNOWN, ONE, TWO, THREE, FOUR
    };

    enum class VertexAttribType
    {
        UNKNOWN, FLOAT, INT
    };

    class VertexBuffer
    {
    public:
        virtual ~VertexBuffer() {}

        virtual void bind() = 0;
        virtual void unBind() = 0;

        VertexAttribType getAttribType() { return m_vertAttribType; }
        VertexAttribSize getAttribSize() { return m_vertAttribSyze; }

    protected:
        VertexAttribType m_vertAttribType = VertexAttribType::UNKNOWN;
        VertexAttribSize m_vertAttribSyze = VertexAttribSize::UNKNOWN;
    };

    class IndexBuffer
    {
    public:
        virtual ~IndexBuffer() {}

        virtual void bind() = 0;
        virtual void unBind() = 0;

        uint32_t getCount() { return m_count; }

    protected:
        uint32_t m_count = 0;
    };
}
