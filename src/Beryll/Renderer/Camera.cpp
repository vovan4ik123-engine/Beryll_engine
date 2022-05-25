#include "Camera.h"

namespace Beryll
{
    glm::vec3 Camera::m_cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 Camera::m_cameraFrontPos = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 Camera::m_cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::vec3 Camera::m_cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 Camera::m_cameraRight = glm::vec3(0.0f, 0.0f, 0.0f);

    float Camera::m_fovRadians = glm::radians(45.0f);
    float Camera::m_perspNearClipPlane = 2.0f;
    float Camera::m_perspFarClipPlane = 500.0f;

    float Camera::m_objectsViewDistance = 100.0f;

    glm::mat4 Camera::m_perspective3D{1.0f};
}