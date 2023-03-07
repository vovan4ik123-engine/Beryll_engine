#include "SimpleCollidingCharacter.h"
#include "Beryll/Renderer/Camera.h"
#include "Beryll/Core/TimeStep.h"

namespace Beryll
{
    SimpleCollidingCharacter::SimpleCollidingCharacter(const char* modelPath,  // common params
                                                       float collisionMass,    // physics params
                                                       bool wantCollisionCallBack,
                                                       CollisionFlags collFlag,
                                                       CollisionGroups collGroup,
                                                       CollisionGroups collMask,
                                                       SceneObjectGroups objGroup)
                                                       // call base class constructor
                                                       : SimpleCollidingObject(modelPath,
                                                                               collisionMass,
                                                                               wantCollisionCallBack,
                                                                               collFlag,
                                                                               collGroup,
                                                                               collMask,
                                                                               objGroup)
    {
        // colliding character described by collision mesh
        m_fromOriginToTop = std::abs(m_mostTopVertex);
        m_fromOriginToBottom = std::abs(m_mostBottomVertex);
        m_characterHeight = m_fromOriginToTop + m_fromOriginToBottom;
        m_XZradius = (std::abs(m_biggestX) + std::abs(m_smallestX)) * 0.5f;
        BR_ASSERT(((m_fromOriginToBottom > 0.0f) && (m_fromOriginToTop > 0.0f) && (m_XZradius > 0.0f) && (m_characterHeight > 0.0f)), "%s", "characters XYZ dimensions are 0.");

        BR_INFO("m_fromOriginToTop:%f, m_fromOriginToBottom:%f, m_characterHeight:%f", m_fromOriginToTop, m_fromOriginToBottom, m_characterHeight);

        m_characterMass = collisionMass;
        m_previousYPos = m_origin.y;
    }

    SimpleCollidingCharacter::~SimpleCollidingCharacter()
    {

    }

    void SimpleCollidingCharacter::updateBeforePhysics()
    {
        // call base class method first
        SimpleCollidingObject::updateBeforePhysics();


    }

    void SimpleCollidingCharacter::updateAfterPhysics()
    {
        // call base class method first
        SimpleCollidingObject::updateAfterPhysics();

        //BR_INFO("origin X:%d Y:%d Z:%d", m_origin.x, m_origin.y, m_origin.z);
        if(!getIsActive())
        {
            return;
        }

        // object is active. that means it should have collisions or drop down in the air

        if(m_characterCanStay)
        {
            m_lastTimeCanStay = TimeStep::getSecFromStart();
        }

        m_characterCanStay = false;
        m_characterMoving = false;

        m_bottomCollisionPoint = std::make_pair(glm::vec3(0.0f, std::numeric_limits<float>::max(), 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));

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

                // point.second is normal vector on collision point
                float floorAngleRadians = Utils::Common::getAngleInRadians(BeryllConstants::worldUp, point.second);
                if(floorAngleRadians < walkableFloorAngleRadians)
                {
                    //BR_INFO("%s", "characterOnGround = true");
                    // character stay on allowed floor angle
                    m_characterCanStay = true;
                    // DONT break loop here !!! continue collect m_bottomCollisionPoint
                }
            }
        }

        if(m_characterCanStay)
        {
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
            if(m_jumped)
            {
                applyImpulse(glm::vec3{0.0f, -1.0f, 0.f} * moveSpeed * startFallingPower);
                m_canApplyStartFallingImpulse = false;
            }
            else if((m_lastTimeCanStay + jumpExtendTime) < TimeStep::getSecFromStart())
            {
                applyImpulse(glm::vec3{0.0f, -1.0f, 0.f} * moveSpeed * startFallingPower);
                m_canApplyStartFallingImpulse = false;
            }
        }

        m_previousYPos = m_origin.y;
    }

    void SimpleCollidingCharacter::draw()
    {
        // call base class method
        SimpleCollidingObject::draw();
    }

    void SimpleCollidingCharacter::playSound()
    {
        // call base class method
        SimpleCollidingObject::playSound();
    }

    void SimpleCollidingCharacter::move(MoveDirection direction)
    {
        glm::quat rotationCharacterToCamera = glm::rotation(m_eyeDirectionXZ, Camera::getCameraDirectionXZ());
        addToRotation(rotationCharacterToCamera);
        // after rotation
        m_eyeDirectionXZ = Camera::getCameraDirectionXZ(); // should be unit
        m_eyeDirectionXYZ = Camera::getCameraDirectionXYZ();
        m_backDirectionXZ = Camera::getCameraBackDirectionXZ();
        m_rightDirectionXZ = Camera::getCameraRightXZ();
        m_leftDirectionXZ = Camera::getCameraLeftXZ();

        glm::vec3 moveVector{0.0f, 0.0f, 0.0f};

        if(direction == MoveDirection::FORWARD)
        {
            moveVector = (m_eyeDirectionXZ * moveSpeed) * TimeStep::getTimeStepSec();
        }
        else if(direction == MoveDirection::BACKWARD)
        {
            moveVector = (m_backDirectionXZ * moveSpeed * backwardMoveFactor) * TimeStep::getTimeStepSec();
        }
        else if(direction == MoveDirection::LEFT)
        {
            moveVector = (m_leftDirectionXZ * moveSpeed) * TimeStep::getTimeStepSec();
        }
        else if(direction == MoveDirection::RIGHT)
        {
            moveVector = (m_rightDirectionXZ * moveSpeed) * TimeStep::getTimeStepSec();
        }

        //BR_INFO("origin X:%f Y:%f Z:%f", m_origin.x, m_origin.y, m_origin.z);
        float moveVectorLength = glm::length(moveVector);
        glm::vec3 scaledMoveDirectionByRadius = ((m_XZradius * 1.3f) / moveVectorLength) * moveVector;

        glm::vec3 characterHeadUp = m_origin;
        characterHeadUp.y += m_fromOriginToTop;
        glm::vec3 characterHeadUpNextPos = characterHeadUp + scaledMoveDirectionByRadius;
        RayClosestHit headWallHit = Physics::castRayClosestHit(characterHeadUp,
                                                               characterHeadUpNextPos,
                                                               m_collisionGroup,
                                                               m_collisionMask);
        if(headWallHit.hit)
        {
            glm::vec3 headBackwardMoveVector = glm::normalize(characterHeadUp - characterHeadUpNextPos);
            if(Utils::Common::getAngleInRadians(headBackwardMoveVector, headWallHit.hitNormal) < 0.698f) // < than 40 degrees
            {
                // characters head moving directly into wall = can not move
                BR_INFO("%s", "headWallHit return");
                return;
            }
        }

        if(m_characterCanStay)
        {
            bool allowedStairStepFound = false;

            glm::vec3 characterLegs = m_origin;
            BR_ASSERT((m_fromOriginToBottom > m_XZradius), "%s", "character origin should be inside cylinder of capsule. Not in bottom semi sphere.");
            characterLegs.y -= (m_fromOriginToBottom - m_XZradius * 0.97f); // characterLegs.y - distance from origin to capsules cylinder bottom
            glm::vec3 characterLegsNextPos = characterLegs + scaledMoveDirectionByRadius;
            RayClosestHit legsWallHit = Physics::castRayClosestHit(characterLegs,
                                                                   characterLegsNextPos,
                                                                   m_collisionGroup,
                                                                   m_collisionMask);
            if(legsWallHit.hit)
            {
                BR_INFO("%s", "legsWallHit");
                glm::vec3 legsBackwardMoveVector = glm::normalize(characterLegs - characterLegsNextPos);
                if(Utils::Common::getAngleInRadians(legsBackwardMoveVector, legsWallHit.hitNormal) < 0.698f) // < than 40 degrees
                {
                    BR_INFO("%s", "legsWallHit < than 40 degrees");
                    // legs hit wall in front
                    // search for stair step
                    glm::vec3 stepCheckUp = glm::vec3(characterLegsNextPos.x, m_origin.y + m_fromOriginToTop, characterLegsNextPos.z);
                    glm::vec3 stepCheckBottom = glm::vec3(characterLegsNextPos.x, m_origin.y - m_fromOriginToBottom, characterLegsNextPos.z);
                    RayClosestHit potentialStepHit = Physics::castRayClosestHit(stepCheckUp,
                                                                                stepCheckBottom,
                                                                                m_collisionGroup,
                                                                                m_collisionMask);

                    if(potentialStepHit.hit)
                    {
                        BR_INFO("potentialStepHit y:%f", potentialStepHit.hitPoint.y);
                        float surfaceSlopeRadians = Utils::Common::getAngleInRadians(BeryllConstants::worldUp, potentialStepHit.hitNormal);
                        if(surfaceSlopeRadians < glm::radians(3.0f)) // allow stair step surface be slope 0-3 degrees
                        {
                            // probably we found stair step
                            // calculate where should be character if that is not stair step, but only ground slope
                            float oppositeSideLength = glm::tan(surfaceSlopeRadians) * glm::length(scaledMoveDirectionByRadius);
                            oppositeSideLength *= 1.02f; // + 2%
                            if(oppositeSideLength == 0.0f) { oppositeSideLength += 0.02f; } // add 2 cm
                            float nextYOfcharacter = m_bottomCollisionPoint.first.y + oppositeSideLength; // after move on this ground slope(if not stair step)
                            if(potentialStepHit.hitPoint.y > nextYOfcharacter)
                            {
                                // assume stair step in front
                                float diffcharacterYStepY = potentialStepHit.hitPoint.y - m_bottomCollisionPoint.first.y;
                                if(diffcharacterYStepY <= maxStepHeight)
                                {
                                    // character can move to this stair step
                                    BR_INFO("%s h:%f", "character moved to stair step", diffcharacterYStepY);
                                    allowedStairStepFound = true;
                                    moveVector.y = diffcharacterYStepY;
                                }
                                else
                                {
                                    // legs hit wall in front but stair step in front is too height
                                    BR_INFO("%s h:%f", "legs hit wall in front but stair step in front is too height", diffcharacterYStepY);
                                    return;
                                }
                            }
                        }
                    }
                    else
                    {
                        // legs hit wall in front but no stair step in front
                        BR_INFO("%s", "legs hit wall in front but no stair step in front");
                        return;
                    }
                }
            }

            if(!allowedStairStepFound)
            {
                // approximate next allowed collision points on Y axis(up and bottom) after character move based on walkableFloorAngle
                // if m_characterCanStay == true, m_bottomCollisionPoint.first.y < std::numeric_limits<float>::max() also should be true
                float approximatedAllowedPositionOnY = glm::tan(walkableFloorAngleRadians) * moveVectorLength;
                approximatedAllowedPositionOnY *= 1.01f; // + 1%

                glm::vec3 newPosApproximatedUp = m_bottomCollisionPoint.first + moveVector;
                newPosApproximatedUp.y += approximatedAllowedPositionOnY;
                glm::vec3 newPosApproximatedBottom = m_bottomCollisionPoint.first + moveVector;
                newPosApproximatedBottom.y -= approximatedAllowedPositionOnY;
                RayClosestHit newPosYHit = Physics::castRayClosestHit(newPosApproximatedUp,
                                                                      newPosApproximatedBottom,
                                                                      m_collisionGroup,
                                                                      m_collisionMask);

                if(newPosYHit.hit)
                {
                    //BR_INFO("%s", "walkable floor found");
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
        }
        else
        {
            moveVector *= airControlFactor;
        }

        addToOrigin(moveVector);

        m_characterMoving = true;
    }

    void SimpleCollidingCharacter::jump()
    {
        if(m_jumped) { return; }

        if(m_characterCanStay || (m_lastTimeCanStay + jumpExtendTime) > TimeStep::getSecFromStart())
        {
            if(m_characterMoving)
            {
                m_jumpDirection = m_eyeDirectionXZ;
                m_jumpDirection.y = glm::tan(startJumpAngleRadians);
            }
            else
            {
                m_jumpDirection = glm::vec3(0.0f, 1.0f, 0.0f);
            }

            applyImpulse(glm::normalize(m_jumpDirection) * moveSpeed * startJumpPower);
            m_jumped = true;
            m_characterCanStay = false;
        }
    }
}