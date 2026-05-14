#pragma once

#include "beryll/core/GameObject.h"
#include "beryll/core/Window.h"

#include "beryll/renderer/Buffer.h"
#include "beryll/renderer/VertexArray.h"
#include "beryll/renderer/Shader.h"
#include "beryll/renderer/Texture.h"

namespace Beryll
{
    // Base class of all GUI objects.
    class GUIObject : public GameObject
    {
    public:
        GUIObject() = delete;
        GUIObject(const glm::vec3& pos, const glm::vec2& widthHeight, bool consumeDownEvent);
        ~GUIObject() override {}

        /*
         * Inherited pure virtual methods are here.
         */

        int pressedFingerID = -100;

        void enable() { m_isEnabled = true; }
        bool getIsEnabled() { return m_isEnabled; }

        void disable()
        {
            m_isEnabled = false;
            m_pressed = false;
            m_touched = false;
            pressedFingerID = -100;
        }

        void updatePositionInPercents(const glm::vec3& pos, bool updateBuffers = true) // Left bottom corner.
        {
            setPositionInPercents(pos);
            if(updateBuffers)
                updateBuffersWithPositions();
        }
        const glm::vec3& getPositionInPercents() const { return m_positionInPercents; }
        const glm::vec3& getPositionInPixels() const { return m_positionInPixels; }
        const glm::vec3& getPositionNormalized() const { return m_positionNormalized; }

        void updateWidthHeightInPercents(const glm::vec2& wh, bool updateBuffers = true)
        {
            setWidthHeightInPercents(wh);
            if(updateBuffers)
                updateBuffersWithPositions();
        }
        const glm::vec2& getWidthHeightInPercents() { return m_widthHeightInPercents; }
        const glm::vec2& getWidthHeightInPixels() { return m_widthHeightInPixels; }
        const glm::vec2& getWidthHeightNormalized() { return m_widthHeightNormalized; }

    protected:
        bool m_isEnabled = true;
        bool m_pressed = false;
        bool m_touched = false;
        bool m_consumeEvent = true;

        std::shared_ptr<VertexBuffer> m_vertexPosBuffer;
        std::shared_ptr<VertexBuffer> m_textureCoordsBuffer;
        std::shared_ptr<IndexBuffer> m_indexBuffer;
        std::unique_ptr<VertexArray> m_vertexArray;
        std::shared_ptr<Shader> m_internalShader;

#if defined(ANDROID)
        // Vertices created as dynamic buffer. Will be updated in updateBuffersWithPositions().
        std::vector<glm::vec3> m_vertices{glm::vec3{0.0f, 0.0f, 0.0f},
                                          glm::vec3{0.0f, 0.0f, 0.0f},
                                          glm::vec3{0.0f, 0.0f, 0.0f},
                                          glm::vec3{0.0f, 0.0f, 0.0f}};

        std::vector<glm::vec2> m_textureCoords{glm::vec2{0.0f, 1.0f}, // Flipped Y for OpenGL.
                                               glm::vec2{1.0f, 1.0f},
                                               glm::vec2{1.0f, 0.0f},
                                               glm::vec2{0.0f, 0.0f}};

        std::vector<uint32_t> m_indices{0,1,2,
                                        2,3,0};
#elif defined(APPLE)

#endif

    private:
        void setPositionInPercents(const glm::vec3& pos);   // Left bottom corner.
        void setWidthHeightInPercents(const glm::vec2& wh); // Size.
        void updateBuffersWithPositions();

        glm::vec3 m_positionInPercents; // X,Y in screen percents (0...100), Z in value as is (0...1).
        glm::vec3 m_positionInPixels; // X,Y in screen resolution, Z in value as is (0...1).
        glm::vec3 m_positionNormalized; // X,Y in 0...1 range, Z in value as is (0...1).

        glm::vec2 m_widthHeightInPercents; // X,Y in screen percents (0...100).
        glm::vec2 m_widthHeightInPixels; // X,Y in screen resolution.
        glm::vec2 m_widthHeightNormalized; // X,Y in 0...1 range.
    };
}
