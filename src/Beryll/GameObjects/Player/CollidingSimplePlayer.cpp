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

        setAngularFactor(glm::vec3(0.0f, 0.0f, 0.0f), true);
    }

    CollidingSimplePlayer::~CollidingSimplePlayer()
    {

    }

    void CollidingSimplePlayer::updateBeforePhysics()
    {
        // call base class method first
        CollidingSimpleObject::updateBeforePhysics();

    }

    void CollidingSimplePlayer::updateAfterPhysics()
    {
        // call base class method first
        CollidingSimpleObject::updateAfterPhysics();

        //BR_INFO("origin X:{0} Y:{1} Z:{2}", m_origin.x, m_origin.y, m_origin.z);

        m_collidingStaticObjects = Physics::getCollisionsWithGroup(m_ID, CollisionGroups::STATIC_ENVIRONMENT);
        if(!m_collidingStaticObjects.empty())
        {
            m_collidingStaticPoints = Physics::getAllCollisionPoints(m_ID, m_collidingStaticObjects);
            for(const std::pair<glm::vec3, glm::vec3>& point : m_collidingStaticPoints)
            {
                playerOnGround = false;
                // point.second is normal vector on collision point
                if(Utils::Common::getAngleInDegrees(Constants::worldUp, point.second) < walkableFloorAngle)
                {
                    // player stay on allowed floor angle
                    playerOnGround = true;
                    break;
                }
            }
        }

        if(playerOnGround)
        {
            disableGravity(true);
            setLinearFactor(glm::vec3(0.0f, 0.0f, 0.0f), true);
        }
        else
        {
            enableGravity(true);
            setLinearFactor(glm::vec3(1.0f, 1.0f, 1.0f), true);
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
        addToRotation(rotationPlayerToCamera, true);
        // after rotation
        m_eyeDirectionXZ = Camera::getCameraDirectionXZ(); // should be unit
        m_eyeDirectionXYZ = Camera::getCameraDirectionXYZ();

        glm::vec3 moveDistance = (m_eyeDirectionXZ * moveSpeed) * TimeStep::getTimeStepSec();
        if(!playerOnGround)
        {
            moveDistance *= airControlFactor;
        }
        addToOrigin(moveDistance);
    }
}