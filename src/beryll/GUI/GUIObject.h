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
        ~GUIObject() override {}

        /*
         * Inherited pure virtual methods are here.
         */

        void enable() { m_isEnabled = true; }
        bool getIsEnabled() { return m_isEnabled; }

        void disable()
        {
            m_isEnabled = false;
            m_pressed = false;
            m_touched = false;
        }

        float leftPos; // In range 0 = 0%...1 = 100% of screen size.
        float topPos;
        float width;
        float height;

        // New GUI vars.
        void updatePositionInPercents(const glm::vec3& pos) // Left bottom corner.
        {
            setPositionInPercents(pos);
            updateBuffersWithPositions();
        }
        const glm::vec3& getPositionInPercents() const { return positionInPercents; }
        const glm::vec3& getPositionInPixels() const { return positionInPixels; }
        const glm::vec3& getPositionNormalized() const { return positionNormalized; }

        void updateWidthHeightInPercents(const glm::vec2& wh)
        {
            setWidthHeightInPercents(wh);
            updateBuffersWithPositions();
        }
        const glm::vec2& getWidthHeightInPercents() { return widthHeightInPercents; }
        const glm::vec2& getWidthHeightInPixels() { return widthHeightInPixels; }
        const glm::vec2& getWidthHeightNormalized() { return widthHeightNormalized; }

    protected:
        // Properties only for GUI objects.
        int32_t m_noBackgroundNoFrame = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                                        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground |
                                        ImGuiWindowFlags_NoScrollbar;

        int32_t m_noFrame = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar;

        const std::string m_IDAsString = std::to_string(m_ID); // Only for GUI elements.

        bool m_isEnabled = true;
        bool m_pressed = false;
        bool m_touched = false;

        // New GUI vars.
        // Left bottom corner.
        glm::vec3 positionInPercents; // X,Y in screen percents (0...100), Z in value as is (0...1).
        glm::vec3 positionInPixels; // X,Y in screen resolution, Z in value as is (0...1).
        glm::vec3 positionNormalized; // X,Y in 0...1 range, Z in value as is (0...1).

        void setPositionInPercents(const glm::vec3& pos) // Left bottom corner.
        {
            BR_ASSERT((pos.x >= 0.0f && pos.x <= 100.0f &&
                       pos.y >= 0.0f && pos.y <= 100.0f &&
                       pos.z >= 0.0f && pos.z <= 1.0f), "%s", "pos must be between 0.0f and 100.0f.");

            positionInPercents = pos;

            // Recalculate only X and Y. Z will always in range 0...1.
            positionNormalized.x = positionInPercents.x / 100.0f;
            positionNormalized.y = positionInPercents.y / 100.0f;
            positionNormalized.z = positionInPercents.z;

            positionInPixels.x = positionNormalized.x * Window::getInstance()->getScreenWidth();
            positionInPixels.y = positionNormalized.y * Window::getInstance()->getScreenHeight();
            positionInPixels.z = positionInPercents.z;
        }

        glm::vec2 widthHeightInPercents; // X,Y in screen percents (0...100).
        glm::vec2 widthHeightInPixels; // X,Y in screen resolution.
        glm::vec2 widthHeightNormalized; // X,Y in 0...1 range.

        void setWidthHeightInPercents(const glm::vec2& wh)
        {
            BR_ASSERT((wh.x >= 0.0f && wh.x <= 100.0f &&
                       wh.y >= 0.0f && wh.y <= 100.0f), "%s", "wh must be between 0.0f and 100.0f.");

            widthHeightInPercents = wh;
            widthHeightNormalized = widthHeightInPercents / 100.0f;
            widthHeightInPixels.x = widthHeightNormalized.x * Window::getInstance()->getScreenWidth();
            widthHeightInPixels.y = widthHeightNormalized.y * Window::getInstance()->getScreenHeight();
        }

        void updateBuffersWithPositions()
        {
            if(m_vertexPosBuffer == nullptr)
            {
                BR_ASSERT(false, "%s", "Buffer is nullptr.");
                return;
            }
            // Move coords to GUI screenSpace -1...1.
            glm::vec3 screenSpacePos = getPositionNormalized();
            screenSpacePos.x = screenSpacePos.x * 2.0f - 1.0f;
            screenSpacePos.y = screenSpacePos.y * 2.0f - 1.0f;
            glm::vec2 WH = getWidthHeightNormalized() * 2.0f;
            std::vector<glm::vec3> vertices{glm::vec3(screenSpacePos.x,         screenSpacePos.y,        screenSpacePos.z),
                                            glm::vec3(screenSpacePos.x + WH.x,  screenSpacePos.y,        screenSpacePos.z),
                                            glm::vec3(screenSpacePos.x + WH.x,  screenSpacePos.y + WH.y, screenSpacePos.z),
                                            glm::vec3(screenSpacePos.x,         screenSpacePos.y + WH.y, screenSpacePos.z)};

            m_vertexPosBuffer->setDynamicBufferData(vertices, vertices.size());
        }

        std::shared_ptr<VertexBuffer> m_vertexPosBuffer;
        std::shared_ptr<VertexBuffer> m_textureCoordsBuffer;
        std::shared_ptr<IndexBuffer> m_indexBuffer;
        std::unique_ptr<VertexArray> m_vertexArray;
        std::shared_ptr<Shader> m_internalShader;
    };
}
