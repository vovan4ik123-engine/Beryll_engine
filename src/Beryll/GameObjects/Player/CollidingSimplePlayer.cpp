#include "CollidingSimplePlayer.h"
#include "Beryll/Renderer/Camera.h"
#include "Beryll/Core/TimeStep.h"

namespace Beryll
{
    CollidingSimplePlayer::CollidingSimplePlayer(const char* modelPath,  // common params
                          float collisionMass,    // physics params
                          bool wantCollisionCallBack,
                          CollisionFlags collFlag,
                          CollisionGroups collGroup,
                          CollisionGroups collMask)
                          // call base class constructor
                          : CollidingSimpleObject(modelPath,
                                                  collisionMass,
                                                  wantCollisionCallBack,
                                                  collFlag,
                                                  collGroup,
                                                  collMask)
    {
        // player described by collision mesh

        m_playerHeight = m_mostTopVertex - m_mostBottomVertex;
        m_XZradius = (m_biggestX - m_smallestX) / 2.0f;
        m_fromOriginToBottom = 0 - m_mostBottomVertex;
        m_fromOriginToTop = m_mostTopVertex - 0;
        BR_ASSERT(((m_fromOriginToBottom > 0.0f) && (m_fromOriginToTop > 0.0f) && (m_XZradius > 0.0f) && (m_playerHeight > 0.0f)), "%s", "Players XYZ dimensions are 0.");

        BR_INFO("m_fromOriginToTop:%f, m_fromOriginToBottom:%f, m_playerHeight:%f", m_fromOriginToTop, m_fromOriginToBottom, m_playerHeight);

        m_playerMass = collisionMass;

        setAngularFactor(glm::vec3(0.0f, 0.0f, 0.0f));
        setLinearFactor(glm::vec3(1.0f, 1.0f, 1.0f));
    }

    CollidingSimplePlayer::~CollidingSimplePlayer()
    {

    }

    void CollidingSimplePlayer::updateBeforePhysics()
    {
        // call base class method first
        CollidingSimpleObject::updateBeforePhysics();

        if(m_playerOnGround)
        {
            disableGravity();
        }
        else
        {
            enableGravity();
        }
    }

    void CollidingSimplePlayer::updateAfterPhysics()
    {
        // call base class method first
        CollidingSimpleObject::updateAfterPhysics();

        //BR_INFO("origin X:%d Y:%d Z:%d", m_origin.x, m_origin.y, m_origin.z);
        if(!getIsActive())
        {
            return;
        }

        // object is active. that means it should have collisions or drop down in the air
        m_playerOnGround = false;
        m_playerMoving = false;
        //BR_INFO("%s", "playerOnGround = false");

        m_bottomCollisionPoint = std::make_pair(glm::vec3(0.0f, std::numeric_limits<float>::max(), 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));

        m_collidingStaticObjects = Physics::getCollisionsWithGroup(m_ID, CollisionGroups::STATIC_ENVIRONMENT);
        if(!m_collidingStaticObjects.empty())
        {
            m_collidingStaticPoints = Physics::getAllCollisionPoints(m_ID, m_collidingStaticObjects);
            for(const std::pair<glm::vec3, glm::vec3>& point : m_collidingStaticPoints)
            {
                if(point.first.y < m_bottomCollisionPoint.first.y)
                {
                    m_bottomCollisionPoint = point;
                }

                // point.second is normal vector on collision point
                float floorAngleRadians = Utils::Common::getAngleInRadians(BeryllConstants::worldUp, point.second);
                if(floorAngleRadians < walkableFloorAngleRadians)
                {
                    //BR_INFO("%s", "playerOnGround = true");
                    // player stay on allowed floor angle
                    m_playerOnGround = true;
                }
            }

            if(m_playerOnGround)
            {
                resetVelocities();
            }
        }

    }

    void CollidingSimplePlayer::draw()
    {
        // call base class method
        CollidingSimpleObject::draw();
    }

    void CollidingSimplePlayer::playSound()
    {
        // call base class method
        CollidingSimpleObject::playSound();
    }

    void CollidingSimplePlayer::move(MoveDirection direction)
    {
        glm::quat rotationPlayerToCamera = glm::rotation(m_eyeDirectionXZ, Camera::getCameraDirectionXZ());
        addToRotation(rotationPlayerToCamera);
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
        glm::vec3 radiusToMoveDirection = ((m_XZradius + m_XZradius * 0.3f) / moveVectorLength) * moveVector;

        glm::vec3 playerHeadUp = m_origin;
        playerHeadUp.y += m_fromOriginToTop;
        glm::vec3 playerHeadUpNextPos = playerHeadUp + radiusToMoveDirection;
        RayClosestHit headWallHit = Physics::castRayClosestHit(playerHeadUp,
                                                                     playerHeadUpNextPos,
                                                                     CollisionGroups::PLAYER,
                                                                     CollisionGroups::STATIC_ENVIRONMENT);
        if(headWallHit.hit)
        {
            glm::vec3 headBackwardMoveVector = glm::normalize(playerHeadUp - playerHeadUpNextPos);
            if(Utils::Common::getAngleInRadians(headBackwardMoveVector, headWallHit.hitNormal) < 0.698f) // < than 40 degrees
            {
                // players head moving directly into wall = can not move
                BR_INFO("%s", "headWallHit return");
                return;
            }
        }

        if(m_playerOnGround)
        {
            bool allowedStairStepFound = false;

            glm::vec3 playerLegs = m_origin;
            playerLegs.y -= (m_fromOriginToBottom - m_XZradius);
            glm::vec3 playerLegsNextPos = playerLegs + radiusToMoveDirection;
            RayClosestHit legsWallHit = Physics::castRayClosestHit(playerLegs,
                                                                   playerLegsNextPos,
                                                                   CollisionGroups::PLAYER,
                                                                   CollisionGroups::STATIC_ENVIRONMENT);
            if(legsWallHit.hit)
            {
                BR_INFO("%s", "legsWallHit");
                glm::vec3 legsBackwardMoveVector = glm::normalize(playerLegs - playerLegsNextPos);
                if(Utils::Common::getAngleInRadians(legsBackwardMoveVector, legsWallHit.hitNormal) < 0.698f) // < than 40 degrees
                {
                    BR_INFO("%s", "legsWallHit < than 40 degrees");
                    // legs hit wall in front
                    // search for stair step
                    glm::vec3 stepCheckUp = glm::vec3(playerLegsNextPos.x, m_origin.y + m_fromOriginToTop, playerLegsNextPos.z);
                    glm::vec3 stepCheckBottom = glm::vec3(playerLegsNextPos.x, m_origin.y - m_fromOriginToBottom, playerLegsNextPos.z);
                    RayClosestHit potentialStepHit = Physics::castRayClosestHit(stepCheckUp,
                                                                                stepCheckBottom,
                                                                                CollisionGroups::PLAYER,
                                                                                CollisionGroups::STATIC_ENVIRONMENT);

                    if (potentialStepHit.hit)
                    {
                        BR_INFO("potentialStepHit y:%f", potentialStepHit.hitPoint.y);
                        float surfaceSlopeRadians = Utils::Common::getAngleInRadians(BeryllConstants::worldUp, potentialStepHit.hitNormal);
                        if (surfaceSlopeRadians < glm::radians(3.0f)) // allow stair step surface be slope 0-3 degrees
                        {
                            // probably we found stair step
                            // calculate where should be player if that is not stair step, but only ground slope
                            float oppositeSideLength = glm::tan(surfaceSlopeRadians) * glm::length(radiusToMoveDirection);
                            oppositeSideLength *= 1.02; // + 2%
                            if(oppositeSideLength == 0.0f) { oppositeSideLength += 0.02f; } // add 2 cm
                            float nextYOfPlayer = m_bottomCollisionPoint.first.y + oppositeSideLength; // after move on this ground slope(if not stair step)
                            if (potentialStepHit.hitPoint.y > nextYOfPlayer)
                            {
                                // assume stair step in front
                                float diffPlayerYStepY = potentialStepHit.hitPoint.y - m_bottomCollisionPoint.first.y;
                                if (diffPlayerYStepY <= maxStepHeight)
                                {
                                    // player can move to this stair step
                                    BR_INFO("%s h:%f", "player moved to stair step", diffPlayerYStepY);
                                    allowedStairStepFound = true;
                                    moveVector.y = diffPlayerYStepY;
                                }
                                else
                                {
                                    // legs hit wall in front but stair step in front is too height
                                    BR_INFO("%s h:%f", "legs hit wall in front but stair step in front is too height", diffPlayerYStepY);
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
                // approximate next allowed collision points on Y axis(up and bottom) after player move based on walkableFloorAngle
                // if m_playerOnGround == true, m_bottomCollisionPoint.first.y < std::numeric_limits<float>::max() also should be true
                float approximatedAllowedPositionOnY = glm::tan(walkableFloorAngleRadians) * moveVectorLength;
                approximatedAllowedPositionOnY *= 1.05f; // + 5%

                glm::vec3 newPosApproximatedUp = m_bottomCollisionPoint.first + moveVector;
                newPosApproximatedUp.y += approximatedAllowedPositionOnY;
                glm::vec3 newPosApproximatedBottom = m_bottomCollisionPoint.first + moveVector;
                newPosApproximatedBottom.y -= approximatedAllowedPositionOnY;
                RayClosestHit newPosYHit = Physics::castRayClosestHit(newPosApproximatedUp,
                                                                      newPosApproximatedBottom,
                                                                      CollisionGroups::PLAYER,
                                                                      CollisionGroups::STATIC_ENVIRONMENT);

                if (newPosYHit.hit)
                {
                    //BR_INFO("%s", "walkable floor found");
                    if (m_bottomCollisionPoint.first.y > newPosYHit.hitPoint.y)
                    {
                        moveVector.y = -(m_bottomCollisionPoint.first.y - newPosYHit.hitPoint.y);
                    } else
                    {
                        moveVector.y = newPosYHit.hitPoint.y - m_bottomCollisionPoint.first.y;
                    }
                }
            }
        }
        else
        {
            moveVector *= airControlFactor;
        }

        addToOrigin(moveVector);

        m_playerMoving = true;
    }

    void CollidingSimplePlayer::jump()
    {
        if(m_playerOnGround)
        {
            if(m_playerMoving)
            {
                m_jumpDirection = m_eyeDirectionXZ;
                m_jumpDirection.y = glm::tan(startJumpAngleRadians);
            }
            else
            {
                m_jumpDirection = glm::vec3(0.0f, 1.0f, 0.0f);
            }

            applyImpulse(glm::normalize(m_jumpDirection) * moveSpeed * startJumpPower);
        }
    }
}