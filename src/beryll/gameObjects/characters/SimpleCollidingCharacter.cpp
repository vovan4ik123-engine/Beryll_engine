#include "SimpleCollidingCharacter.h"
#include "beryll/core/TimeStep.h"

namespace Beryll
{
    SimpleCollidingCharacter::SimpleCollidingCharacter(const char* filePath,  // Common params.
                                                       float collisionMassKg,    // Physics params.
                                                       bool wantCollisionCallBack,
                                                       CollisionFlags collFlag,
                                                       CollisionGroups collGroup,
                                                       CollisionGroups collMask,
                                                       SceneObjectGroups sceneGroup)
                                                       // Call base class constructor.
                                                       : SimpleCollidingObject(filePath,
                                                                               collisionMassKg,
                                                                               wantCollisionCallBack,
                                                                               collFlag,
                                                                               collGroup,
                                                                               collMask,
                                                                               sceneGroup)
    {
        BR_ASSERT(((collFlag == CollisionFlags::DYNAMIC && wantCollisionCallBack == true) ||
                   (collFlag != CollisionFlags::DYNAMIC)), "%s", "collFlag DYNAMIC must have wantCollisionCallBack = true to use character controller.");

        // Colliding character described by collision mesh.
        m_fromOriginToTop = std::abs(m_mostTopVertex);
        m_fromOriginToBottom = std::abs(m_mostBottomVertex);
        m_characterHeight = m_fromOriginToTop + m_fromOriginToBottom;
        m_XZRadius = (std::abs(m_biggestX) + std::abs(m_smallestX)) * 0.5f;
        BR_ASSERT((m_fromOriginToBottom > 0.0f && m_fromOriginToTop > 0.0f && m_XZRadius > 0.0f && m_characterHeight > 0.0f), "%s", "Characters XYZ dimensions are 0.");

        BR_INFO("m_fromOriginToTop: %f, m_fromOriginToBottom: %f, m_characterHeight: %f", m_fromOriginToTop, m_fromOriginToBottom, m_characterHeight);

        m_characterMass = collisionMassKg;
        m_previousYPos = m_origin.y;
    }

    SimpleCollidingCharacter::~SimpleCollidingCharacter()
    {

    }

    void SimpleCollidingCharacter::updateBeforePhysics()
    {
        // Call base class method first.
        SimpleCollidingObject::updateBeforePhysics();
    }

    void SimpleCollidingCharacter::updateAfterPhysics()
    {
        // Call base class method first.
        SimpleCollidingObject::updateAfterPhysics();

        if(m_collisionFlag != CollisionFlags::DYNAMIC) { return; }

        //BR_INFO("origin X: %d Y: %d Z: %d", m_origin.x, m_origin.y, m_origin.z);
        if(!getIsActive())
        {
            return;
        }

        // Object is active. That means it should have collisions or drop down in the air.

        if(m_characterCanStay)
        {
            m_lastTimeCanStay = TimeStep::getSecFromStart();
        }

        m_characterCanStay = false;
        m_characterMoving = false;

        m_bottomCollisionPoint = std::make_pair(glm::vec3(0.0f, std::numeric_limits<float>::max(), 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));

        if(m_collisionMask != CollisionGroups::NONE)
        {
            m_collidingObjects = Physics::getCollisionsWithGroup(m_ID, m_collisionMask);
            if(!m_collidingObjects.empty())
            {
                m_collidingPoints = Physics::getAllCollisionPoints(m_ID, m_collidingObjects);
                for(const std::pair<glm::vec3, glm::vec3>& point : m_collidingPoints)
                {
                    if(point.first.y < m_bottomCollisionPoint.first.y)
                    {
                        m_bottomCollisionPoint = point;
                    }

                    // point.second is normal vector on collision point.
                    float floorAngleRadians = BeryllUtils::Common::getAngleInRadians(BeryllConstants::worldUp, point.second);
                    if(floorAngleRadians < walkableFloorAngleRadians)
                    {
                        //BR_INFO("%s", "characterOnGround = true");
                        // Character stay on allowed floor angle.
                        m_characterCanStay = true;
                        // DONT break loop here !!! Continue collect m_bottomCollisionPoint.
                    }
                }
            }
        }

        if(m_characterCanStay)
        {
            resetVelocities();

            m_jumped = false;
            m_falling = false;
            m_canApplyStartFallingImpulse = true;
        }
        else if(m_previousYPos > m_origin.y)
        {
            m_falling = true;
        }
        else
        {
            m_falling = false;
        }

        if(m_canApplyStartFallingImpulse && m_falling)
        {
            if(m_jumped || (m_lastTimeCanStay + jumpExtendTime) < TimeStep::getSecFromStart())
            {
                applyCentralImpulse(glm::vec3{0.0f, -1.0f, 0.f} * moveSpeed * startFallingPower);
                m_canApplyStartFallingImpulse = false;
            }
        }

        m_previousYPos = m_origin.y;
    }

    void SimpleCollidingCharacter::draw()
    {
        // Call base class method first.
        SimpleCollidingObject::draw();
    }

    void SimpleCollidingCharacter::moveToPosition(const glm::vec3& position, bool rotateWhenMove, bool ignoreYAxisWhenRotate)
    {
        const glm::vec3 needToMove = position - m_origin;

        if(rotateWhenMove)
            rotateToDirection(needToMove, ignoreYAxisWhenRotate);

        glm::vec3 moveVector = glm::normalize(needToMove) * moveSpeed * TimeStep::getTimeStepSec();

        if(glm::length(moveVector) > glm::length(needToMove))
            // Object should move less distance than he can.
            moveVector = needToMove;

        move(moveVector);
    }

    void SimpleCollidingCharacter::moveToDirection(glm::vec3 direction, bool rotateWhenMove, bool ignoreYAxisWhenRotate)
    {
        direction = glm::normalize(direction);

        if(rotateWhenMove)
            rotateToDirection(direction, ignoreYAxisWhenRotate);

        move(direction * moveSpeed * TimeStep::getTimeStepSec());
    }

    void SimpleCollidingCharacter::move(glm::vec3 moveVector)
    {
        if(m_collisionFlag != CollisionFlags::DYNAMIC)
        {
            addToOrigin(moveVector);
            return;
        }

        //BR_INFO("origin X: %f Y: %f Z: %f", m_origin.x, m_origin.y, m_origin.z);
        float moveVectorLength = glm::length(moveVector);
        glm::vec3 scaledMoveDirectionByRadius = ((m_XZRadius * 1.3f) / moveVectorLength) * moveVector;

        glm::vec3 characterHeadUp = m_origin;
        characterHeadUp.y += m_fromOriginToTop;
        glm::vec3 characterHeadUpNextPos = characterHeadUp + scaledMoveDirectionByRadius;
        RayClosestHit headSomethingHit = Physics::castRayClosestHit(characterHeadUp,
                                                                    characterHeadUpNextPos,
                                                                    m_collisionGroup,
                                                                    m_collisionMask);
        if(headSomethingHit)
        {
            glm::vec3 headBackwardMoveVector = glm::normalize(characterHeadUp - characterHeadUpNextPos);
            if(BeryllUtils::Common::getAngleInRadians(headBackwardMoveVector, headSomethingHit.hitNormal) < 0.698f && // < than 40 degrees.
               headSomethingHit.collFlag != CollisionFlags::DYNAMIC)
            {
                // Characters head moving directly into static wall = can not move.
                BR_INFO("%s", "headSomethingHit hit not DYNAMIC object, return.");
                return;
            }
        }

        if(m_characterCanStay)
        {
            bool allowedStairStepFound = false;

            glm::vec3 characterLegs = m_origin;
            characterLegs.y -= (m_fromOriginToBottom - m_XZRadius * 0.98f); // characterLegs.y - distance from origin to capsules cylinder bottom.
            glm::vec3 characterLegsNextPos = characterLegs + scaledMoveDirectionByRadius;
            RayClosestHit legsSomethingHit = Physics::castRayClosestHit(characterLegs,
                                                                        characterLegsNextPos,
                                                                        m_collisionGroup,
                                                                        m_collisionMask);
            if(legsSomethingHit)
            {
                BR_INFO("%s", "legsSomethingHit");
                glm::vec3 legsBackwardMoveVector = glm::normalize(characterLegs - characterLegsNextPos);
                if(BeryllUtils::Common::getAngleInRadians(legsBackwardMoveVector, legsSomethingHit.hitNormal) < 0.698f && // < than 40 degrees.
                   legsSomethingHit.collFlag != CollisionFlags::DYNAMIC)
                {
                    BR_INFO("%s", "legsSomethingHit < than 40 degrees.");
                    // Legs hit something static in front.
                    // Search for stair step.
                    glm::vec3 stepCheckUp = glm::vec3(characterLegsNextPos.x, m_origin.y + m_fromOriginToTop, characterLegsNextPos.z);
                    glm::vec3 stepCheckBottom = glm::vec3(characterLegsNextPos.x, m_origin.y - m_fromOriginToBottom, characterLegsNextPos.z);
                    RayClosestHit potentialStepHit = Physics::castRayClosestHit(stepCheckUp,
                                                                                stepCheckBottom,
                                                                                m_collisionGroup,
                                                                                m_collisionMask);

                    if(potentialStepHit)
                    {
                        BR_INFO("potentialStepHit y: %f", potentialStepHit.hitPoint.y);
                        float surfaceSlopeRadians = BeryllUtils::Common::getAngleInRadians(BeryllConstants::worldUp, potentialStepHit.hitNormal);
                        if(surfaceSlopeRadians < glm::radians(3.0f)) // Allow stair step surface be slope 0-3 degrees.
                        {
                            // Probably we found stair step.
                            // Calculate where should be character if that is not stair step, but only ground slope.
                            float oppositeSideLength = glm::tan(surfaceSlopeRadians) * glm::length(scaledMoveDirectionByRadius);
                            oppositeSideLength *= 1.02f; // Add 2%.
                            if(oppositeSideLength == 0.0f) { oppositeSideLength += 0.02f; } // Add 2 cm.
                            float nextYOfCharacter = m_bottomCollisionPoint.first.y + oppositeSideLength; // After move on this ground slope(if not stair step).
                            if(potentialStepHit.hitPoint.y > nextYOfCharacter)
                            {
                                // Assume stair step in front.
                                float diffCharacterYStepY = potentialStepHit.hitPoint.y - m_bottomCollisionPoint.first.y;
                                if(diffCharacterYStepY <= maxStepHeight)
                                {
                                    // Character can move to this stair step.
                                    BR_INFO("%s h: %f", "character moved to stair step", diffCharacterYStepY);
                                    allowedStairStepFound = true;
                                    moveVector.y = diffCharacterYStepY;
                                }
                                else
                                {
                                    // Legs hit static wall in front but stair step in front is too height.
                                    BR_INFO("%s h: %f", "legs hit static wall in front but stair step in front is too height", diffCharacterYStepY);
                                    return;
                                }
                            }
                        }
                    }
                    else
                    {
                        // Legs hit static wall in front but no stair step in front.
                        BR_INFO("%s", "legs hit static wall in front but no stair step in front");
                        return;
                    }
                }
            }

            if(!allowedStairStepFound)
            {
                // Approximate next allowed collision points on Y axis(up and bottom) after character move based on walkableFloorAngle.
                // If m_characterCanStay == true, m_bottomCollisionPoint.first.y < std::numeric_limits<float>::max() also should be true.
                float approximatedAllowedPositionOnY = glm::tan(walkableFloorAngleRadians) * moveVectorLength;
                approximatedAllowedPositionOnY *= 1.01f; // Add 1%.

                glm::vec3 newPosApproximatedUp = m_bottomCollisionPoint.first + moveVector;
                newPosApproximatedUp.y += approximatedAllowedPositionOnY;
                glm::vec3 newPosApproximatedBottom = m_bottomCollisionPoint.first + moveVector;
                newPosApproximatedBottom.y -= approximatedAllowedPositionOnY;
                RayClosestHit newPosYHit = Physics::castRayClosestHit(newPosApproximatedUp,
                                                                      newPosApproximatedBottom,
                                                                      m_collisionGroup,
                                                                      m_collisionMask);

                if(newPosYHit)
                {
                    //BR_INFO("%s", "Walkable floor found.");
                    if(m_bottomCollisionPoint.first.y > newPosYHit.hitPoint.y)
                    {
                        moveVector.y = -(m_bottomCollisionPoint.first.y - newPosYHit.hitPoint.y);
                    }
                    else
                    {
                        moveVector.y = newPosYHit.hitPoint.y - m_bottomCollisionPoint.first.y;
                    }
                }
                else
                {
                    BR_INFO("%s", "Not walkable floor angle.");
                }
            }

            addToOrigin(moveVector);

            m_characterMoving = true;
            m_jumpDirection = moveVector;
        }
        else if(m_falling)
        {
            addToOrigin(moveVector * airControlFactor);
        }
    }

    void SimpleCollidingCharacter::jump()
    {
        if(m_collisionFlag != CollisionFlags::DYNAMIC) { return; }

        if(m_jumped) { return; }

        if(m_characterCanStay || (m_lastTimeCanStay + jumpExtendTime) > TimeStep::getSecFromStart())
        {
            if(m_characterMoving)
            {
                m_jumpDirection.y = glm::tan(startJumpAngleRadians);
            }
            else
            {
                m_jumpDirection = glm::vec3(0.0f, 1.0f, 0.0f);
            }

            applyCentralImpulse(glm::normalize(m_jumpDirection) * moveSpeed * startJumpPower);
            m_jumped = true;
            m_characterCanStay = false;
        }
    }
}
