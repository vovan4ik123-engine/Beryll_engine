#include "CharacterController.h"
#include "beryll/gameObjects/SceneObject.h"
#include "beryll/core/TimeStep.h"

namespace Beryll
{
    CharacterController::CharacterController(SceneObject* objUnderControl)
    {
        m_sceneObject = objUnderControl;
    }

    CharacterController::~CharacterController()
    {

    }

    void CharacterController::update()
    {
        if(m_firstUpdate)
        {
            m_lastTimeOnGround = TimeStep::getSecFromStart();
            // This controller will apply gravity.
            Physics::setGravityForObject(m_sceneObject->getID(), glm::vec3(0.0f), false, false);
            m_firstUpdate = false;
        }

        if(m_sceneObject->getCollisionFlag() != CollisionFlags::DYNAMIC)
            return;

        m_moving = false;

        if(!m_sceneObject->getIsActive())
        {
            m_lastTimeOnGround = TimeStep::getSecFromStart();
            m_canJump = true;
            m_applyJumpImpulse = false;
            return;
        }

        // Object is dynamic and active.

        m_canStay = false;
        m_bottomCollisionPoint = std::make_pair(glm::vec3(0.0f, std::numeric_limits<float>::max(), 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        m_collidingObjects = Physics::getAllCollisionsForIDWithGroup(m_sceneObject->getID(), m_sceneObject->getCollisionMask());

        if(!m_collidingObjects.empty())
        {
            m_collidingPoints = Physics::getAllCollisionPoints(m_sceneObject->getID(), m_collidingObjects);
            for(const std::pair<glm::vec3, glm::vec3>& point : m_collidingPoints)
            {
                if(point.first.y < m_bottomCollisionPoint.first.y)
                    m_bottomCollisionPoint = point;

                // point.second is normal vector on collision point.
                const float floorAngleRadians = BeryllUtils::Common::getAngleInRadians(BeryllConstants::worldUp, point.second);
                if(floorAngleRadians < walkableFloorAngleRadians)
                {
                    // Character touch allowed floor/object angle.
                    //BR_INFO("%s", "m_canStay == true");
                    m_canStay = true;
                    m_canJump = true;
                    m_applyJumpImpulse = false;

                    m_lastTimeOnGround = TimeStep::getSecFromStart();
                    // DONT break loop here !!! Continue collect m_bottomCollisionPoint.
                }
            }
        }

        if(m_canJump && m_lastTimeOnGround + jumpExtendTime >= TimeStep::getSecFromStart())
            m_canJump = true;
        else
            m_canJump = false;

        // Apply gravity.
        const float fallingTime = TimeStep::getSecFromStart() - m_lastTimeOnGround;
        const glm::vec3 fallingVelocity = m_sceneObject->getGravity() * std::max(fallingTime, 0.01f);
        glm::vec3 newOrigin = m_sceneObject->getOrigin() + (fallingVelocity * Beryll::TimeStep::getTimeStepSec());
        if(m_applyJumpImpulse)
            newOrigin += m_jumpImpulse * Beryll::TimeStep::getTimeStepSec();
        m_sceneObject->setOrigin(newOrigin);
        //BR_INFO("grav %f", (fallingVelocity * Beryll::TimeStep::getTimeStepSec()).y);
    }

    void CharacterController::moveToPosition(const glm::vec3& position, bool rotateWhenMove, bool ignoreYAxisWhenRotate)
    {
        const glm::vec3 needToMove = position - m_sceneObject->getOrigin();

        if(glm::any(glm::isnan(needToMove)) || glm::length(needToMove) == 0.0f)
        {
            m_moving = false;
            return;
        }

        if(rotateWhenMove)
            m_sceneObject->rotateToDirection(needToMove, ignoreYAxisWhenRotate);

        glm::vec3 moveVector = glm::normalize(needToMove) * moveSpeed * TimeStep::getTimeStepSec();

        if(glm::length(moveVector) > glm::length(needToMove)) // Object should move less distance than he can.
            moveVector = needToMove;

        move(moveVector);
    }

    void CharacterController::moveToDirection(glm::vec3 direction, bool rotateWhenMove, bool ignoreYAxisWhenRotate)
    {
        if(glm::any(glm::isnan(direction)) || glm::length(direction) == 0.0f)
            return;

        direction = glm::normalize(direction);

        if(rotateWhenMove)
            m_sceneObject->rotateToDirection(direction, ignoreYAxisWhenRotate);

        move(direction * moveSpeed * TimeStep::getTimeStepSec());
    }

    void CharacterController::move(const glm::vec3& moveVector)
    {
        //BR_INFO("origin X: %f Y: %f Z: %f", m_sceneObject->getOrigin().x, m_sceneObject->getOrigin().y, m_sceneObject->getOrigin().z);
        m_moveDir = glm::normalize(moveVector);
        if(m_sceneObject->getCollisionFlag() != CollisionFlags::DYNAMIC)
        {
            m_sceneObject->addToOrigin(moveVector);
            m_moving = true;
            return;
        }

        glm::vec3 newOrigin = m_sceneObject->getOrigin() + moveVector;

        const glm::vec3 moveVectorXZ = glm::vec3(moveVector.x, 0.0f, moveVector.z);
        glm::vec3 newBottomCollisionPoint = m_bottomCollisionPoint.first + moveVectorXZ;
        glm::vec3 nextPosMaxUp = newBottomCollisionPoint;
        nextPosMaxUp.y += m_sceneObject->getObjectHeight();
        glm::vec3 nextPosMinBottom = newBottomCollisionPoint;
        nextPosMinBottom.y -= m_sceneObject->getObjectHeight();
        const RayClosestHit newPosYHit = Physics::castRayClosestHit(nextPosMaxUp, nextPosMinBottom,
                                                                    m_sceneObject->getCollisionGroup(),
                                                                    m_sceneObject->getCollisionMask());

        if(newPosYHit)
        {
            const float walkableFloorMaxDistance = glm::tan(walkableFloorAngleRadians) * glm::length(moveVectorXZ);
            const float yDistanceToHitPoint = glm::distance(m_bottomCollisionPoint.first.y, newPosYHit.hitPoint.y);
            if(yDistanceToHitPoint < walkableFloorMaxDistance)
            {
                newOrigin.y += (newPosYHit.hitPoint.y - m_bottomCollisionPoint.first.y) - 0.01f; // - 0.01f <- make sure collision happens.
            }
        }

        m_sceneObject->setOrigin(newOrigin);
        m_moving = true;
    }

    bool CharacterController::jump(const glm::vec3& impulse)
    {
        if(!m_canJump || m_sceneObject->getCollisionFlag() != CollisionFlags::DYNAMIC)
            return false;

        m_jumpImpulse = impulse;
        m_applyJumpImpulse = true;
        m_sceneObject->addToOrigin(m_jumpImpulse * Beryll::TimeStep::getTimeStepSec());
        m_canJump = false;

        return true;
    }
}
