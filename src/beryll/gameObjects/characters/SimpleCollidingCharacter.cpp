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
        BR_INFO("m_fromOriginToTop: %f, m_fromOriginToBottom: %f, m_XZRadius: %f, m_characterHeight: %f", m_fromOriginToTop, m_fromOriginToBottom, m_XZRadius, m_characterHeight);

        BR_ASSERT((m_fromOriginToBottom > 0.0f && m_fromOriginToTop > 0.0f && m_XZRadius > 0.0f && m_characterHeight > 0.0f), "%s", "Characters XYZ dimensions are 0.");

        moveSpeed = (m_characterHeight / 1.8f) * 3.0f; // For human 1.8m height average speed is 3m|s.
        maxStepHeight = m_characterHeight * 0.2f;
        startJumpPower = collisionMassKg;
        startFallingPower = collisionMassKg * 0.5f;

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

        // Object is active. That means it should have collisions or moves(up or down) in the air.

        m_characterCanStay = false;
        m_characterMoving = false;
        m_touchGroundAfterFall = false;

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
                        // Character stay on allowed floor angle.
                        //BR_INFO("%s", "characterCanStay = true 1");
                        m_characterCanStay = true;
                        m_lastTimeOnGround = TimeStep::getSecFromStart();

                        if(m_falling)
                        {
                            m_touchGroundAfterFall = true;
                            m_fallDistance = glm::distance(m_startFallingHeight, m_origin.y - m_fromOriginToBottom);
                        }
                        // DONT break loop here !!! Continue collect m_bottomCollisionPoint.
                    }
                }
            }
            else if(!m_jumped && m_lastTimeOnGround + canStayOrJumpExtendTime >= TimeStep::getSecFromStart())
            {
                m_characterCanStay = true;
            }
        }

        if(m_characterCanStay)
        {
            resetVelocities();

            m_jumped = false;
            m_falling = false;
            m_startFalling = false;
        }
        else if(m_previousYPos > m_origin.y) // Character falling.
        {
            if(!m_startFalling)
            {
                m_startFalling = true;
                m_startFallingHeight = m_origin.y - m_fromOriginToBottom; // Bottom Y position.

                applyCentralImpulse(glm::vec3{0.0f, -1.0f, 0.f} * moveSpeed * startFallingPower);
            }

            m_falling = true;
            m_fallDistance = glm::distance(m_startFallingHeight, m_origin.y - m_fromOriginToBottom);
        }
        // else {} Character flying up. Probably after jump or maybe explosion happened under character.

        m_previousYPos = m_origin.y;
    }

    void SimpleCollidingCharacter::draw()
    {
        // Call base class method first.
        SimpleCollidingObject::draw();
    }

    void SimpleCollidingCharacter::moveToPosition(const glm::vec3& position, bool rotateWhenMove, bool ignoreYAxisWhenRotate, bool pushDynamicObjects)
    {
        const glm::vec3 needToMove = position - m_origin;

        if(rotateWhenMove)
            rotateToDirection(needToMove, ignoreYAxisWhenRotate);

        glm::vec3 moveVector = glm::normalize(needToMove) * moveSpeed * TimeStep::getTimeStepSec();

        if(glm::length(moveVector) > glm::length(needToMove)) // Object should move less distance than he can.
            moveVector = needToMove;

        move(moveVector, pushDynamicObjects);
    }

    void SimpleCollidingCharacter::moveToDirection(glm::vec3 direction, bool rotateWhenMove, bool ignoreYAxisWhenRotate, bool pushDynamicObjects)
    {
        direction = glm::normalize(direction);

        if(rotateWhenMove)
            rotateToDirection(direction, ignoreYAxisWhenRotate);

        move(direction * moveSpeed * TimeStep::getTimeStepSec(), pushDynamicObjects);
    }

    void SimpleCollidingCharacter::move(glm::vec3 moveVector, bool pushDynamicObjects)
    {
        //BR_INFO("origin X: %f Y: %f Z: %f", m_origin.x, m_origin.y, m_origin.z);
        if(m_collisionFlag != CollisionFlags::DYNAMIC)
        {
            addToOrigin(moveVector);
            return;
        }

        bool headHitObstacle = false; // Obstacle which can not be pushed. So we should stop player or move along wall.
        glm::vec3 headObstacleNormal{0.0f};
        bool headFrontObstacle = false;
        bool headLeftObstacle = false;
        bool headRightObstacle = false;
        const float moveVectorLength = glm::length(moveVector);

        glm::vec3 characterHeadUp = m_origin;
        characterHeadUp.y += m_fromOriginToTop;
        glm::vec3 characterHeadFront = characterHeadUp + ((m_XZRadius * 2.3f) / moveVectorLength) * moveVector;
        RayClosestHit headFrontHit = Physics::castRayClosestHit(characterHeadUp, characterHeadFront, m_collisionGroup, m_collisionMask);
        if(headFrontHit &&
           (headFrontHit.collFlag != CollisionFlags::DYNAMIC || (headFrontHit.collFlag == CollisionFlags::DYNAMIC && !pushDynamicObjects)))
        {
            //BR_INFO("%s", "headFrontHit");
            headFrontObstacle = true;
            headHitObstacle = true;
            headObstacleNormal = headFrontHit.hitNormal;
        }

        glm::vec3 moveVectorLeftSide = glm::rotate(glm::normalize(moveVector), glm::quarter_pi<float>(), BeryllConstants::worldUp) * moveVectorLength;
        glm::vec3 characterHeadLeft = characterHeadUp + ((m_XZRadius * 2.0f) / moveVectorLength) * moveVectorLeftSide;
        RayClosestHit headLeftHit = Physics::castRayClosestHit(characterHeadUp, characterHeadLeft, m_collisionGroup, m_collisionMask);
        if(headLeftHit &&
           (headLeftHit.collFlag != CollisionFlags::DYNAMIC || (headLeftHit.collFlag == CollisionFlags::DYNAMIC && !pushDynamicObjects)))
        {
            //BR_INFO("%s", "headLeftHit");
            headLeftObstacle = true;

            if(!headFrontObstacle)
            {
                headHitObstacle = true;
                headObstacleNormal = headLeftHit.hitNormal;
            }
        }

        glm::vec3 moveVectorRightSide =
                glm::rotate(glm::normalize(moveVector), -glm::quarter_pi<float>(), BeryllConstants::worldUp) * moveVectorLength;
        glm::vec3 characterHeadRight = characterHeadUp + ((m_XZRadius * 2.0f) / moveVectorLength) * moveVectorRightSide;
        RayClosestHit headRightHit = Physics::castRayClosestHit(characterHeadUp, characterHeadRight, m_collisionGroup, m_collisionMask);
        if(headRightHit &&
           (headRightHit.collFlag != CollisionFlags::DYNAMIC || (headRightHit.collFlag == CollisionFlags::DYNAMIC && !pushDynamicObjects)))
        {
            //BR_INFO("%s", "headRightHit");
            headRightObstacle = true;

            if(!headFrontObstacle && !headLeftObstacle)
            {
                headHitObstacle = true;
                headObstacleNormal = headRightHit.hitNormal;
            }
        }

        if(headFrontObstacle && headLeftObstacle && headRightObstacle)
        {
            //BR_INFO("%s", "headFrontObstacle && headLeftObstacle && headRightObstacle. Can not move. return.");
            return;
        }

        if(headHitObstacle)
        {
            glm::vec3 headFrontBackwardVector = glm::normalize(characterHeadUp - characterHeadFront);
            float backwardMoveToNormalAngle = BeryllUtils::Common::getAngleInRadians(headFrontBackwardVector, headObstacleNormal);
            if(backwardMoveToNormalAngle < 0.5236) // < than 30 degrees.
            {
                // Characters head moving into wall = can not move.
                //BR_INFO("%s", "headFrontHit hit something that we can not push, return.");
                return;
            }
            else
            {
                //BR_INFO("%s", "Move along wall.");
                if(BeryllUtils::Common::getIsVectorOnRightSide(headObstacleNormal, glm::normalize(moveVector)))
                    moveVector = BeryllUtils::Common::getRightVector(headObstacleNormal) * moveVectorLength;
                else
                    moveVector = BeryllUtils::Common::getLeftVector(headObstacleNormal) * moveVectorLength;
            }
        }

        if(m_jumped || !m_characterCanStay)
        {
            addToOrigin(moveVector * airControlFactor);
            return;
        }

        // Here m_characterCanStay == true.

        bool allowedStairStepFound = false;
        bool somethingHitInFront = false;
        glm::vec3 somethingHitPoint{0.0f};
        glm::vec3 somethingHitNormal{0.0f};
        CollisionFlags somethingHitCollFlag = CollisionFlags::NONE;
        float directionScaledByRadiusLength = 0.0f;
        float characterTopY = m_origin.y + m_fromOriginToTop;
        float characterBottomY = m_origin.y - m_fromOriginToBottom;

        glm::vec3 frontDirectionScaledByRadius = ((m_XZRadius * 1.5f) / moveVectorLength) * moveVector;
        glm::vec3 characterBodyMoveFront = m_origin + frontDirectionScaledByRadius;
        glm::vec3 stepCheckUp = glm::vec3(characterBodyMoveFront.x, characterTopY, characterBodyMoveFront.z);
        glm::vec3 stepCheckBottom = glm::vec3(characterBodyMoveFront.x, characterBottomY, characterBodyMoveFront.z);
        stepCheckBottom.y += 0.005f; // A bit upped than player bottom to avoid hit flat ground.
        RayClosestHit somethingHitOnFront = Physics::castRayClosestHit(stepCheckUp, stepCheckBottom, m_collisionGroup, m_collisionMask);
        if(somethingHitOnFront)
        {
            //BR_INFO("%s", "somethingHitOnFront");
            somethingHitInFront = true;
            somethingHitPoint = somethingHitOnFront.hitPoint;
            somethingHitNormal = somethingHitOnFront.hitNormal;
            somethingHitCollFlag = somethingHitOnFront.collFlag;
            directionScaledByRadiusLength = glm::length(frontDirectionScaledByRadius);
        }
        else // Check left side.
        {
            moveVectorLeftSide = glm::rotate(glm::normalize(moveVector), glm::quarter_pi<float>(), BeryllConstants::worldUp) * moveVectorLength;
            glm::vec3 leftDirectionScaledByRadius = ((m_XZRadius * 1.5f) / moveVectorLength) * moveVectorLeftSide;
            glm::vec3 characterBodyMoveLeft = m_origin + leftDirectionScaledByRadius;
            stepCheckUp = glm::vec3(characterBodyMoveLeft.x, characterTopY, characterBodyMoveLeft.z);
            stepCheckBottom = glm::vec3(characterBodyMoveLeft.x, characterBottomY, characterBodyMoveLeft.z);
            stepCheckBottom.y += 0.005f; // A bit upped than player bottom to avoid hit flat ground.
            RayClosestHit somethingHitOnLeft = Physics::castRayClosestHit(stepCheckUp, stepCheckBottom, m_collisionGroup, m_collisionMask);
            if(somethingHitOnLeft)
            {
                //BR_INFO("%s", "somethingHitOnLeft");
                somethingHitInFront = true;
                somethingHitPoint = somethingHitOnLeft.hitPoint;
                somethingHitNormal = somethingHitOnLeft.hitNormal;
                somethingHitCollFlag = somethingHitOnLeft.collFlag;
                directionScaledByRadiusLength = glm::length(leftDirectionScaledByRadius);
            }
            else // Check right side.
            {
                moveVectorRightSide = glm::rotate(glm::normalize(moveVector), -glm::quarter_pi<float>(), BeryllConstants::worldUp) * moveVectorLength;
                glm::vec3 rightDirectionScaledByRadius = ((m_XZRadius * 1.5f) / moveVectorLength) * moveVectorRightSide;
                glm::vec3 characterBodyMoveRight = m_origin + rightDirectionScaledByRadius;
                stepCheckUp = glm::vec3(characterBodyMoveRight.x, characterTopY, characterBodyMoveRight.z);
                stepCheckBottom = glm::vec3(characterBodyMoveRight.x, characterBottomY, characterBodyMoveRight.z);
                stepCheckBottom.y += 0.005f; // A bit upped than player bottom to avoid hit flat ground.
                RayClosestHit somethingHitOnRight = Physics::castRayClosestHit(stepCheckUp, stepCheckBottom, m_collisionGroup, m_collisionMask);
                if(somethingHitOnRight)
                {
                    //BR_INFO("%s", "somethingHitOnRight");
                    somethingHitInFront = true;
                    somethingHitPoint = somethingHitOnRight.hitPoint;
                    somethingHitNormal = somethingHitOnRight.hitNormal;
                    somethingHitCollFlag = somethingHitOnRight.collFlag;
                    directionScaledByRadiusLength = glm::length(rightDirectionScaledByRadius);
                }
            }
        }

        if(somethingHitInFront)
        {
            //BR_INFO("Something hit in front at height: %f, check for stair step or ground slope.", somethingHitPoint.y - characterBottomY);
            // Probably we found stair step.
            // Calculate where should be character if that is not stair step, but only ground slope.
            float surfaceSlopeRadians = BeryllUtils::Common::getAngleInRadians(BeryllConstants::worldUp, somethingHitNormal);
            float oppositeSideLength = glm::tan(surfaceSlopeRadians) * directionScaledByRadiusLength;
            oppositeSideLength *= 1.01f; // Add 1%.
            if(oppositeSideLength == 0.0f) { oppositeSideLength += 0.01f; } // Add 1 cm.
            float nextYOfCharacter = characterBottomY + oppositeSideLength; // After move on this ground slope(if not stair step).
            //BR_INFO("%s", somethingHitPoint.y > nextYOfCharacter ? "That is something like stair step." : "That is ground slope.");
            if(somethingHitPoint.y > nextYOfCharacter &&
               (somethingHitCollFlag != CollisionFlags::DYNAMIC || (somethingHitCollFlag == CollisionFlags::DYNAMIC && !pushDynamicObjects)))
            {
                // Stair step in front + we can not push object.
                float diffStepHeightCharacterBottom = somethingHitPoint.y - characterBottomY;
                if(diffStepHeightCharacterBottom <= maxStepHeight)
                {
                    // Character can move to this stair step.
                    //BR_INFO("%s", "Character moved to stair step because can not push object and stair step height is allowed.");
                    allowedStairStepFound = true;
                    moveVector.y = diffStepHeightCharacterBottom;
                }
                else
                {
                    // Stair step in front is too height.
                    //BR_INFO("%s", "Stair step in front is too height and we can not push object, return");
                    return;
                }
            }
        }

        if(!allowedStairStepFound)
        {
            // Approximate next allowed collision points on Y axis(up and bottom) after character move based on walkableFloorAngle.
            // m_characterCanStay == true, m_bottomCollisionPoint.first.y < std::numeric_limits<float>::max() both should be true.
            float walkableFloorMaxHeight = glm::tan(walkableFloorAngleRadians) * moveVectorLength;

            glm::vec3 walkableFloorMaxUp = m_bottomCollisionPoint.first + moveVector;
            walkableFloorMaxUp.y += walkableFloorMaxHeight;
            glm::vec3 walkableFloorMinBottom = m_bottomCollisionPoint.first + moveVector;
            walkableFloorMinBottom.y -= walkableFloorMaxHeight;
            RayClosestHit newPosYHit = Physics::castRayClosestHit(walkableFloorMaxUp, walkableFloorMinBottom, m_collisionGroup, m_collisionMask);

            if(newPosYHit)
            {
                //BR_INFO("%s", "Walkable floor found.");
                if(m_bottomCollisionPoint.first.y > newPosYHit.hitPoint.y)
                    moveVector.y = -(m_bottomCollisionPoint.first.y - newPosYHit.hitPoint.y);
                else
                    moveVector.y = newPosYHit.hitPoint.y - m_bottomCollisionPoint.first.y;
            }
        }

        addToOrigin(moveVector);

        m_characterMoving = true;
        m_jumpDirection = moveVector; // Be careful. Y can be != 0.0f and length != 1.0f.
    }

    bool SimpleCollidingCharacter::jump()
    {
        if(m_collisionFlag != CollisionFlags::DYNAMIC || m_jumped) { return false; }

        if(m_characterCanStay)
        {
            if(m_characterMoving)
            {
                m_jumpDirection.y = 0.0f;
                m_jumpDirection = glm::normalize(m_jumpDirection);
                m_jumpDirection.y = glm::tan(startJumpAngleRadians);
                applyCentralImpulse((glm::normalize(m_jumpDirection) * moveSpeed * startJumpPower) * 1.8f);
            }
            else
            {
                // Jump up if stay.
                applyCentralImpulse(glm::vec3(0.0f, 1.0f, 0.0f) * moveSpeed * startJumpPower);
            }

            m_jumped = true;
            m_previousYPos = m_origin.y; // Reset falling mechanism. m_falling calculated based on that.

            return true;
        }

        return false;
    }
}
