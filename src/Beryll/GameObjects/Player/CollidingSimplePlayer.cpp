#include "CollidingSimplePlayer.h"
#include "Beryll/Renderer/Camera.h"

namespace Beryll
{
    CollidingSimplePlayer::CollidingSimplePlayer(const char* modelPath,  // common params
                          bool canBeDisabled,
                          float collisionMass,    // physics params
                          bool wantCollisionCallBack,
                          CollisionFlags collFlag,
                          CollisionGroups collGroup,
                          CollisionGroups collMask,
                          const char* vertexPath, // graphics params
                          const char* fragmentPath,
                          const char* diffSampler,
                          const char* specSampler)
                          // call base class constructor
                          : CollidingSimpleObject(modelPath,
                                                  canBeDisabled,
                                                  collisionMass,
                                                  wantCollisionCallBack,
                                                  collFlag,
                                                  collGroup,
                                                  collMask,
                                                  vertexPath,
                                                  fragmentPath,
                                                  diffSampler,
                                                  specSampler)
    {
        // player described by collision mesh

        for (int i = 0; i < m_scene->mNumMeshes; ++i)
        {
            std::string meshName = m_scene->mMeshes[i]->mName.C_Str();

            if (meshName.find("Collision") != std::string::npos)
            {
                float smallestX = 0.0f;
                float biggestX = 0.0f;
                float mostBottomVertex = 0.0f;
                float mostTopVertex = 0.0f;
                for (int g = 0; g < m_scene->mMeshes[i]->mNumVertices; ++g)
                {
                    if (m_scene->mMeshes[i]->mVertices[g].y < mostBottomVertex)
                    {
                        mostBottomVertex = m_scene->mMeshes[i]->mVertices[g].y;
                    }

                    if (m_scene->mMeshes[i]->mVertices[g].y > mostTopVertex)
                    {
                        mostTopVertex = m_scene->mMeshes[i]->mVertices[g].y;
                    }

                    if (m_scene->mMeshes[i]->mVertices[g].x < smallestX)
                    {
                        smallestX = m_scene->mMeshes[i]->mVertices[g].x;
                    }
                    if (m_scene->mMeshes[i]->mVertices[g].x > biggestX)
                    {
                        biggestX = m_scene->mMeshes[i]->mVertices[g].x;
                    }
                }

                m_playerHeight = mostTopVertex - mostBottomVertex;
                m_XZradius = (biggestX - smallestX) / 2.0f;
                m_fromOriginToBottom = 0 - mostBottomVertex;
                m_fromOriginToTop = mostTopVertex - 0;
            }
        }

        BR_INFO("m_fromOriginToTop:{0}, m_fromOriginToBottom:{1}, m_playerHeight{2}", m_fromOriginToTop, m_fromOriginToBottom, m_playerHeight);

        BR_ASSERT(((m_fromOriginToBottom > 0.0f) && (m_fromOriginToTop > 0.0f) && (m_XZradius > 0.0f) && (m_playerHeight > 0.0f)), "Players XYZ dimensions are 0.");

        m_walkableFloorAngleRadians = glm::radians(walkableFloorAngleDegrees);

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
            disableGravity(true);
        }
        else
        {
            enableGravity(true);
        }
    }

    void CollidingSimplePlayer::updateAfterPhysics()
    {
        // call base class method first
        CollidingSimpleObject::updateAfterPhysics();

        //BR_INFO("origin X:{0} Y:{1} Z:{2}", m_origin.x, m_origin.y, m_origin.z);
        if(!getIsActive())
        {
            return;
        }

        // object is active. that means it should have collisions or drop down in the air
        m_playerOnGround = false;

        m_bottomCollisionPoint = std::make_pair(glm::vec3(0.0f, std::numeric_limits<float>::max(), 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));

        m_collidingStaticObjects = Physics::getCollisionsWithGroup(m_ID, CollisionGroups::STATIC_ENVIRONMENT);
        if(!m_collidingStaticObjects.empty())
        {
            resetVelocities();

            m_collidingStaticPoints = Physics::getAllCollisionPoints(m_ID, m_collidingStaticObjects);
            for(const std::pair<glm::vec3, glm::vec3>& point : m_collidingStaticPoints)
            {
                if(point.first.y < m_bottomCollisionPoint.first.y)
                {
                    m_bottomCollisionPoint = point;
                }

                // point.second is normal vector on collision point
                if(Utils::Common::getAngleInRadians(Constants::worldUp, point.second) < m_walkableFloorAngleRadians)
                {
                    //BR_INFO("playerOnGround = true");
                    // player stay on allowed floor angle
                    m_playerOnGround = true;
                }
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
        glm::quat rotationPlayerToCamera = Utils::Common::getRotationBetweenVectors(m_eyeDirectionXZ, Camera::getCameraDirectionXZ());
        addToRotation(rotationPlayerToCamera);
        // after rotation
        m_eyeDirectionXZ = Camera::getCameraDirectionXZ(); // should be unit
        m_eyeDirectionXYZ = Camera::getCameraDirectionXYZ();
        m_backDirectionXZ = Camera::getCameraBackDirectionXZ();
        m_rightDirectionXZ = Camera::getCameraRightXZ();
        m_leftDirectionXZ = Camera::getCameraLeftXZ();

        glm::vec3 moveDistance{0.0f};

        if(direction == MoveDirection::FORWARD)
        {
            moveDistance = (m_eyeDirectionXZ * moveSpeed) * TimeStep::getTimeStepSec();
        }
        else if(direction == MoveDirection::BACKWARD)
        {
            moveDistance = (m_backDirectionXZ * moveSpeed) * TimeStep::getTimeStepSec();
        }
        else if(direction == MoveDirection::LEFT)
        {
            moveDistance = (m_leftDirectionXZ * moveSpeed) * TimeStep::getTimeStepSec();
        }
        else if(direction == MoveDirection::RIGHT)
        {
            moveDistance = (m_rightDirectionXZ * moveSpeed) * TimeStep::getTimeStepSec();
        }

        if(m_playerOnGround)
        {
            // approximate next allowed collision points on Y axis(up and bottom) after player move based on walkableFloorAngle
            // if m_playerOnGround == true, m_bottomCollisionPoint.first.y < std::numeric_limits<float>::max() also should be true
            float approximatedAllowedPositionOnY = glm::tan(m_walkableFloorAngleRadians) * glm::length(moveDistance);
            approximatedAllowedPositionOnY *= 1.1f; // + 10%

            glm::vec3 newPosApproximatedUp = m_bottomCollisionPoint.first + moveDistance;
            newPosApproximatedUp.y += approximatedAllowedPositionOnY;
            glm::vec3 newPosApproximatedDown = m_bottomCollisionPoint.first + moveDistance;
            newPosApproximatedDown.y -= approximatedAllowedPositionOnY;
            RayClosestHit newPosYHit = Physics::castRayClosestHit(newPosApproximatedUp, newPosApproximatedDown, CollisionGroups::PLAYER, CollisionGroups::STATIC_ENVIRONMENT);

            if(newPosYHit.hit)
            {
                if(m_bottomCollisionPoint.first.y > newPosYHit.hitPoint.y)
                {
                    moveDistance.y = -(m_bottomCollisionPoint.first.y - newPosYHit.hitPoint.y);
                }
                else
                {
                    moveDistance.y = newPosYHit.hitPoint.y - m_bottomCollisionPoint.first.y;
                }
            }
        }
        else
        {
            moveDistance *= airControlFactor;
        }

        addToOrigin(moveDistance);

        m_playerOnGround = false;
    }
}