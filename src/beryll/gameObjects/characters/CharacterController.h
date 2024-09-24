#pragma once

#include "CppHeaders.h"
#include "LibsHeaders.h"

namespace Beryll
{
    class SceneObject;

    class CharacterController final
    {
    public:
        CharacterController() = delete;
        ~CharacterController();

        CharacterController(const CharacterController& cc) = delete;
        CharacterController(CharacterController&& cc) = delete;
        CharacterController& operator=(const CharacterController& cc) = delete;
        CharacterController& operator=(CharacterController&& cc) = delete;

        void moveToDirection(glm::vec3 direction, bool rotateWhenMove, bool ignoreYAxisWhenRotate);
        void moveToPosition(const glm::vec3& position, bool rotateWhenMove, bool ignoreYAxisWhenRotate);
        bool jump(const glm::vec3& impulse);

        bool getIsCanStay() { return m_canStay; }
        bool getIsMoving() { return m_moving; }
        const glm::vec3& getMoveDir() { return m_moveDir; }

        float moveSpeed = 5.0f; // Meters in sec.
        float walkableFloorAngleRadians = glm::radians(50.1f);
        float jumpExtendTime = 0.3f; // In seconds. Time after leave ground but still can jump.

    private:
        friend class SimpleCollidingCharacter;
        friend class AnimatedCollidingCharacter;
        CharacterController(SceneObject* objUnderControl); // Can be created only in SimpleCollidingCharacter/AnimatedCollidingCharacter classes.
        SceneObject* m_sceneObject; // Object under control.
        void update();

        bool m_firstUpdate = true;

        bool m_canStay = false; // Can stay on any colliding object from group m_collisionMask.
        float m_lastTimeOnGround = 0.0f;
        bool m_moving = false;
        bool m_canJump = false;
        glm::vec3 m_moveDir{0.0f};
        glm::vec3 m_jumpImpulse{0.0f};
        bool m_applyJumpImpulse = false;

        std::vector<const int> m_collidingObjects; // Prevent creation and deletion every frame.
        std::vector<std::pair<glm::vec3, glm::vec3>> m_collidingPoints; // Prevent creation and deletion every frame.

        std::pair<glm::vec3, glm::vec3> m_bottomCollisionPoint; // Lowest collision point with ground ant its normal.

        void move(const glm::vec3& moveVector);
    };
}
