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

        virtual void bind() = 0; // Must be called only inside VAO
        virtual void unBind() = 0; // Must be called only inside VAO

        virtual void setDynamicBufferData(const std::vector<float>& data) = 0; // Only for dynamic buffer

        VertexAttribType getAttribType() { return m_vertAttribType; }
        VertexAttribSize getAttribSize() { return m_vertAttribSyze; }

    protected:
        VertexAttribType m_vertAttribType = VertexAttribType::UNKNOWN;
        VertexAttribSize m_vertAttribSyze = VertexAttribSize::UNKNOWN;
    };

    // Dynamic index buffer can be achieved by changing m_count inside setCount(uint32_t count)
    class IndexBuffer
    {
    public:
        virtual ~IndexBuffer() {}

        virtual void bind() = 0;
        virtual void unBind() = 0;

        uint32_t getCount() { return m_count; }
        virtual void setCount(uint32_t count) = 0;

    protected:
        uint32_t m_count = 0;
    };
}
