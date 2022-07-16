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


        m_eyeDirection_X_Y_Z = Camera::getCameraDirection();
        m_eyeDirection_X_Z = glm::normalize(glm::vec3(m_eyeDirection_X_Y_Z.x, 0.0f, m_eyeDirection_X_Y_Z.z));
    }

    CollidingSimplePlayer::~CollidingSimplePlayer()
    {

    }

    void CollidingSimplePlayer::updateBeforePhysics()
    {
        // call base class method first
        CollidingSimpleObject::updateBeforePhysics();

        //Beryll::Physics::activateObject(m_ID);


    }

    void CollidingSimplePlayer::updateAfterPhysics()
    {
        // call base class method first
        CollidingSimpleObject::updateAfterPhysics();


        m_collidingObjectsIDs = Physics::getCollisionsWithGroup(m_ID, CollisionGroups::GROUND);
        if(!m_collidingObjectsIDs.empty())
        {
            //BR_INFO("m_collidingObjectsIDs.size():{0}", m_collidingObjectsIDs.size());
            m_collidingPoints = Physics::getAllCollisionPoints(m_ID, m_collidingObjectsIDs);
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

}