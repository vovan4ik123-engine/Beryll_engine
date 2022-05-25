#pragma once

#include "LibsHeaders.h"

namespace Beryll
{
    class Camera
    {
    public:
        Camera() = delete;
        ~Camera() = delete;

        // update3DCamera() must be called before
        static const glm::mat4& get3DCamera()
        {
            return m_perspective3D;
        }

        /*
        static glm::mat4 getCamera2D(const float& screnWidth, const float& screenHeight)
        {
            // for 2D objects:
            // near plan: Z = 1.0f
            // far plan: Z = 0.0f (NOT -100.0 !!!!!!!)
            return glm::ortho(0.0f, screnWidth, 0.0f, screenHeight, -1.1f, 100.0f);
        }
        */

        // call before get3DCamera()
        static void update3DCamera(const float& screnWidth, const float& screenHeight)
        {
            updateCameraVectors();

            m_perspective3D = getPerspective3D(screnWidth, screenHeight) * getVeiw3D();
        }

        static bool getIsSeeObject(const glm::vec3& objectPos) // check does camera see object or object is out of view
        {
            // check distance
            if(glm::distance(m_cameraPos, objectPos) > m_objectsViewDistance) return false;
            // check angle. here we can use half of fov but that is not enough if objects in corner of screen
            if(glm::acos(glm::dot(m_cameraDirection, glm::normalize(objectPos - m_cameraPos))) > m_fovRadians) return false;

            return true;
        }

        static float getDistanceToObject(const glm::vec3& objectPos) // check distance between camera and object
        {
            return glm::distance(m_cameraPos, objectPos);
        }

        static void setCameraPos(const glm::vec3& pos) { m_cameraPos = pos; }
        static void setCameraFront(const glm::vec3& front) { m_cameraFrontPos = front; }
        static void setCameraFov(const float& fovDegree) { m_fovRadians = glm::radians(fovDegree); }
        static void setPerspectiveNearClipPlane(const float& near) { m_perspNearClipPlane = near; }
        static void setPerspectiveFarClipPlane(const float& far) { m_perspFarClipPlane = far; }
        static void setObjectsViewDistance(const float& viewDistance) { m_objectsViewDistance = viewDistance; }

        static float getObjectsViewDistance() { return m_objectsViewDistance; }
        static const glm::vec3& getCameraDirection() { return m_cameraDirection; }

    private:
        static glm::vec3 m_cameraPos;
        static glm::vec3 m_cameraFrontPos;
        static glm::vec3 m_cameraUp;

        static glm::vec3 m_cameraDirection;
        static glm::vec3 m_cameraRight;

        static float m_fovRadians;
        static float m_perspNearClipPlane;
        static float m_perspFarClipPlane;

        static float m_objectsViewDistance; // for method isSeeObject()

        constexpr static glm::vec3 m_worldUp = glm::vec3(0.0f, 1.0f, 0.0f); // length must be 1

        static glm::mat4 m_perspective3D;

        static void updateCameraVectors()
        {
            m_cameraDirection = glm::normalize(m_cameraFrontPos - m_cameraPos);
            m_cameraRight = glm::normalize(glm::cross(m_cameraDirection, m_worldUp));
            m_cameraUp = glm::normalize(glm::cross(m_cameraRight, m_cameraDirection));
        }

        static glm::mat4 getVeiw3D() // updateCameraVectors() MUST be called before this method !!!
        {
            return glm::lookAt(m_cameraPos, m_cameraPos + m_cameraDirection, m_cameraUp);
        }

        static glm::mat4 getPerspective3D(const float& screenWidth, const float& screenHeight)
        {
            return glm::perspectiveFov(m_fovRadians,
                                       screenWidth,
                                       screenHeight,
                                       m_perspNearClipPlane,
                                       m_perspFarClipPlane);
        }
    };
}