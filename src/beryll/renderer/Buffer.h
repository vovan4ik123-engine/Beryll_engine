#pragma once

#include "LibsHeaders.h"
#include "CppHeaders.h"

namespace Beryll
{
    enum class VertexAttribSize
    {
        UNKNOWN, ONE, TWO, THREE, FOUR, MATRIX4x4
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

        // Only for dynamic buffer
        // elementsCount = data.size() for copy all buffer
        virtual void setDynamicBufferData(const std::vector<glm::vec3>& data, uint32_t elementsCount) = 0;
        virtual void setDynamicBufferData(const std::vector<glm::vec4>& data, uint32_t elementsCount) = 0;
        virtual void setDynamicBufferData(const std::vector<glm::mat4>& data, uint32_t elementsCount) = 0;

        VertexAttribType getAttribType() { return m_vertAttribType; }
        VertexAttribSize getAttribSize() { return m_vertAttribSize; }

    protected:
        VertexAttribType m_vertAttribType = VertexAttribType::UNKNOWN;
        VertexAttribSize m_vertAttribSize = VertexAttribSize::UNKNOWN;
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
