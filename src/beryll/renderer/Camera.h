#pragma once

#include "LibsHeaders.h"
#include "CppHeaders.h"

#include "beryll/core/Window.h"
#include "beryll/utils/CommonUtils.h"

namespace Beryll
{
    class Camera
    {
    public:
        Camera() = delete;
        ~Camera() = delete;

        // update3DCamera() must be called before
        static const glm::mat4& getViewProjection()
        {
            return m_viewProjection;
        }
        static const glm::mat4& getProjection()
        {
            return m_projection;
        }
        static const glm::mat4& getView()
        {
            return m_view;
        }

        static glm::mat4 getLoadingScreenCamera()
        {
            // loading image near plan: Z = 1.0f
            // loading image far plan: Z = 0.0f
            return m_loadingScreenCamera;
        }

        // call before getViewProjection() / getProjection() / getView()
        static void update3DCamera()
        {
            updateCameraVectors();

            m_projection = getProjectionPerspective3D(Window::getInstance()->getScreenWidth(), Window::getInstance()->getScreenHeight());
            m_view = getVeiw3D();

            m_viewProjection = m_projection * m_view;
        }

        static void updateCameraVectors()
        {
            m_cameraDirectionXYZ = glm::normalize(m_cameraFrontPos - m_cameraPos);
            m_cameraDirectionXZ = glm::normalize(glm::vec3(m_cameraDirectionXYZ.x, 0.0f, m_cameraDirectionXYZ.z));

            m_cameraBackDirectionXYZ = -m_cameraDirectionXYZ;
            m_cameraBackDirectionXZ = -m_cameraDirectionXZ;

            m_cameraRightXYZ = glm::normalize(glm::cross(m_cameraDirectionXYZ, BeryllConstants::worldUp));
            m_cameraRightXZ = glm::normalize(glm::vec3(m_cameraRightXYZ.x, 0.0f, m_cameraRightXYZ.z));

            m_cameraLeftXYZ = -m_cameraRightXYZ;
            m_cameraLeftXZ = -m_cameraRightXZ;

            m_cameraUp = glm::normalize(glm::cross(m_cameraRightXYZ, m_cameraDirectionXYZ));
        }

        // check does camera see object or object is out of view
        static bool getIsSeeObject(const glm::vec3& objectPos, float fovMultiplier = 1.0f, float maxViewDistance = m_objectsViewDistance)
        {
            // check distance
            if(glm::distance(m_cameraPos, objectPos) > maxViewDistance) { return false; }

            if(Window::getInstance()->currentOrientation == SDL_ORIENTATION_LANDSCAPE ||
               Window::getInstance()->currentOrientation == SDL_ORIENTATION_LANDSCAPE_FLIPPED)
            {
                if(BeryllUtils::Common::getAngleInRadians(m_cameraDirectionXYZ, glm::normalize(objectPos - m_cameraPos)) > (m_fovRadians * fovMultiplier))
                {
                    return false;
                }
            }
            else if(Window::getInstance()->currentOrientation == SDL_ORIENTATION_PORTRAIT ||
                    Window::getInstance()->currentOrientation == SDL_ORIENTATION_PORTRAIT_FLIPPED)
            {
                if(BeryllUtils::Common::getAngleInRadians(m_cameraDirectionXYZ, glm::normalize(objectPos - m_cameraPos)) > (m_halfFovRadians * fovMultiplier))
                {
                    return false;
                }
            }
            else
            {
                return false;
            }

            return true;
        }

        static float getDistanceToObject(const glm::vec3& objectPos) // check distance between camera and object
        {
            return glm::distance(m_cameraPos, objectPos);
        }

        static void setCameraPos(const glm::vec3& pos) { m_cameraPos = pos; }
        static const glm::vec3& getCameraPos() { return m_cameraPos; }
        static const glm::vec3& getCameraUp() { return m_cameraUp; }

        static void setCameraFront(const glm::vec3& front) { m_cameraFrontPos = front; }
        static void setCameraFov(const float fovDegrees)
        {
            m_fovRadians = glm::radians(fovDegrees);
            m_halfFovRadians = m_fovRadians * 0.5f;
        }
        static void setProjectionNearClipPlane(const float near) { m_projNearClipPlane = near; }
        static void setProjectionFarClipPlane(const float far) { m_projFarClipPlane = far; }
        static void setObjectsViewDistance(const float viewDistance) { m_objectsViewDistance = viewDistance; }

        static float getObjectsViewDistance() { return m_objectsViewDistance; }
        static float getProjectionFarClipPlane() { return m_projFarClipPlane; }
        static const glm::vec3& getCameraDirectionXYZ() { return m_cameraDirectionXYZ; }
        static const glm::vec3& getCameraDirectionXZ() { return m_cameraDirectionXZ; }
        static const glm::vec3& getCameraBackDirectionXYZ() { return m_cameraBackDirectionXYZ; }
        static const glm::vec3& getCameraBackDirectionXZ() { return m_cameraBackDirectionXZ; }
        static const glm::vec3& getCameraRightXYZ() { return m_cameraRightXYZ; }
        static const glm::vec3& getCameraRightXZ() { return m_cameraRightXZ; }
        static const glm::vec3& getCameraLeftXYZ() { return m_cameraLeftXYZ; }
        static const glm::vec3& getCameraLeftXZ() { return m_cameraLeftXZ; }

    private:
        static glm::vec3 m_cameraPos;
        static glm::vec3 m_cameraFrontPos;
        static glm::vec3 m_cameraUp;

        static glm::vec3 m_cameraDirectionXYZ;
        static glm::vec3 m_cameraDirectionXZ;

        static glm::vec3 m_cameraBackDirectionXYZ;
        static glm::vec3 m_cameraBackDirectionXZ;

        static glm::vec3 m_cameraRightXYZ;
        static glm::vec3 m_cameraRightXZ;

        static glm::vec3 m_cameraLeftXYZ;
        static glm::vec3 m_cameraLeftXZ;

        static float m_fovRadians;
        static float m_halfFovRadians;
        static float m_projNearClipPlane;
        static float m_projFarClipPlane;

        static float m_objectsViewDistance; // for method isSeeObject()

        static glm::mat4 m_viewProjection;
        static glm::mat4 m_projection;
        static glm::mat4 m_view;

        static glm::mat4 m_loadingScreenCamera;

        static glm::mat4 getVeiw3D() // updateCameraVectors() MUST be called before this method !!!
        {
            return glm::lookAt(m_cameraPos, m_cameraPos + m_cameraDirectionXYZ, m_cameraUp);
        }

        static glm::mat4 getProjectionPerspective3D(const float screenWidth, const float screenHeight)
        {
            return glm::perspectiveFov(m_fovRadians,
                                       screenWidth,
                                       screenHeight,
                                       m_projNearClipPlane,
                                       m_projFarClipPlane);
        }
    };
}
