#include "Camera.h"

namespace Beryll
{
    glm::vec3 Camera::m_cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 Camera::m_cameraFrontPos = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 Camera::m_cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::vec3 Camera::m_cameraDirectionXYZ = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 Camera::m_cameraDirectionXZ = glm::vec3(0.0f, 0.0f, -1.0f);

    glm::vec3 Camera::m_cameraBackDirectionXYZ = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 Camera::m_cameraBackDirectionXZ = glm::vec3(0.0f, 0.0f, 1.0f);

    glm::vec3 Camera::m_cameraRightXYZ = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 Camera::m_cameraRightXZ = glm::vec3(1.0f, 0.0f, 0.0f);

    glm::vec3 Camera::m_cameraLeftXYZ = glm::vec3(-1.0f, 0.0f, 0.0f);
    glm::vec3 Camera::m_cameraLeftXZ = glm::vec3(-1.0f, 0.0f, 0.0f);

    float Camera::m_fovRadians = glm::radians(45.0f);
    float Camera::m_halfFovRadians = glm::radians(22.5f);
    float Camera::m_perspNearClipPlane = 2.0f;
    float Camera::m_perspFarClipPlane = 500.0f;

    float Camera::m_objectsViewDistance = 100.0f;

    glm::mat4 Camera::m_viewProjection{1.0f};
    glm::mat4 Camera::m_projection{1.0f};
    glm::mat4 Camera::m_view{1.0f};

    glm::mat4 Camera::m_loadingScreenCamera = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 100.0f) *
                                              glm::lookAt(glm::vec3(0.0f, 0.0f, 2.1f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}