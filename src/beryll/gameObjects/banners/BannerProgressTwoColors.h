#pragma once

#include "beryll/renderer/Buffer.h"
#include "beryll/renderer/VertexArray.h"
#include "beryll/renderer/Shader.h"

namespace Beryll
{
    // Represents 2D banner on 3D scene with 2 colors and progress 0%...100%
    // Color1 is at left side, color2 is on right side.
    // Progress value separate left color from right color
    class BannerProgressTwoColors
    {
    public:
        BannerProgressTwoColors() = delete;
        BannerProgressTwoColors(float width, float height, const glm::vec3& color1, const glm::vec3& color2);
        ~BannerProgressTwoColors();

        void draw();

        float progress = 0.0f;

        void setOrigin(const glm::vec3& orig)
        {
            if(m_origin == orig) { return; }

            m_origin = orig;
            m_translateMatrix = glm::translate(glm::mat4{1.0f}, m_origin);

            m_modelMatrix[3][0] = m_origin.x;
            m_modelMatrix[3][1] = m_origin.y;
            m_modelMatrix[3][2] = m_origin.z;
        }

        void addToRotation(const glm::quat& qua)
        {
            if(glm::angle(qua) < 0.0035f) { return; } // less that 0.2 degree

            m_rotateMatrix = glm::toMat4(qua) * m_rotateMatrix;
            // translate and scale matrices should be same
            m_modelMatrix = m_translateMatrix * m_rotateMatrix;
        }

        glm::vec3 getFaceDirXYZ()
        {
            return glm::normalize(glm::vec3(m_rotateMatrix * glm::vec4(m_bannerFaceDir, 1.0f)));
        }

        glm::vec3 getUpDirXYZ()
        {
            return glm::normalize(glm::vec3(m_rotateMatrix * glm::vec4(m_bannerUpDir, 1.0f)));
        }

    private:
        float m_width = 0.0f;
        float m_height = 0.0f;
        float m_leftXPos = 0.0f;
        float m_RightXPos = 0.0f;
        glm::vec3 m_colorLeft{0.0f, 0.0f, 0.0f};
        glm::vec3 m_colorRight{1.0f, 1.0f, 1.0f};

        glm::vec3 m_origin{0.0f, 0.0f, 0.0f};

        glm::mat4 m_rotateMatrix{1.0f};
        glm::mat4 m_translateMatrix{1.0f};

        glm::mat4 m_modelMatrix{1.0f};
        glm::mat4 m_MVP{1.0f};

        std::shared_ptr<VertexBuffer> m_vertexPosBuffer;
        std::shared_ptr<IndexBuffer> m_indexBuffer;
        std::unique_ptr<VertexArray> m_vertexArray;
        std::shared_ptr<Shader> m_internalShader; // Default, simple shader. Use if no shader was bound on scene

        // Banner was not loaded from Blender. We can specify any faceDir and upDir when create it.
        const glm::vec3 m_bannerFaceDir{0.0f, 0.0f, -1.0f};
        const glm::vec3 m_bannerUpDir{0.0f, 1.0f, 0.0f};
    };
}
