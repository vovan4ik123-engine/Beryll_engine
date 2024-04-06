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

        void moveToDirection(glm::vec3 direction, bool rotateWhenMove, bool ignoreYAxisWhenRotate, bool pushDynamicObjects);
        void moveToPosition(const glm::vec3& position, bool rotateWhenMove, bool ignoreYAxisWhenRotate, bool pushDynamicObjects);
        bool jump();

        bool getIsCanStay() { return m_canStay; }
        bool getIsMoving() { return m_moving; }
        bool getIsJumped() { return m_jumped; }
        bool getIsFalling() { return m_falling; }
        bool getIsTouchGroundAfterFall() { return m_touchGroundAfterFall; }
        float getFallDistance() { return m_fallDistance; }

        float moveSpeed = 5.0f; // Meters in sec.
        float backwardMoveFactor = 0.6f; // Factor to multiply moveSpeed if character move backward.
        float walkableFloorAngleRadians = glm::radians(50.0f);
        float maxStepHeight = 1.0f;
        float startJumpAngleRadians = glm::radians(40.0f);
        float startJumpPower = 1.0f;
        float airControlFactor = 0.3f; // Factor to multiply moveSpeed if jumped UP !!! or falling.
        float jumpExtendTime = 0.3f; // In seconds. Time after leave ground but still can jump.

    private:
        SceneObject* m_sceneObject; // Pointer to object under control.

        friend class SimpleCollidingCharacter;
        friend class AnimatedCollidingCharacter;
        CharacterController(SceneObject* objUnderControl); // Can be created only in SimpleCollidingCharacter/AnimatedCollidingCharacter classes.
        void update();

        bool m_canStay = false; // Can stay on any colliding object from group m_collisionMask.
        float m_lastTimeOnGround = 0.0f;
        bool m_moving = false;
        bool m_controllingInAir = false; // By user/player (joystick or keys).
        bool m_touchGroundAfterFall = false;
        bool m_resetVelocities = false;

        float m_previousYPos = 0.0f;

        glm::vec3 m_jumpDirection{0.0f, 1.0f, 0.0f};
        bool m_canJump = false;
        bool m_jumped = false;
        bool m_jumpedWhileMoving = false;
        bool m_falling = false;
        bool m_startFalling = false;
        float m_startFallingHeight = 0.0f;
        float m_fallDistance = 0.0f;

        std::vector<const int> m_collidingObjects; // Prevent creation and deletion every frame.
        std::vector<std::pair<glm::vec3, glm::vec3>> m_collidingPoints; // Prevent creation and deletion every frame.

        std::pair<glm::vec3, glm::vec3> m_bottomCollisionPoint; // Lowest collision point with ground ant its normal.

        void move(glm::vec3 moveVector, bool pushDynamicObjects);
    };
}
