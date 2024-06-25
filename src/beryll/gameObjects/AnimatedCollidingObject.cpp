#include "AnimatedCollidingObject.h"

namespace Beryll
{
    AnimatedCollidingObject::AnimatedCollidingObject(const char* filePath,
                                                     float collisionMassKg,
                                                     bool wantCollisionCallBack,
                                                     CollisionFlags collFlag,
                                                     CollisionGroups collGroup,
                                                     CollisionGroups collMask,
                                                     SceneObjectGroups sceneGroup)
                                                     : BaseAnimatedObject(filePath, sceneGroup)
    {
        BR_INFO("Process animated colliding object: %s", filePath);

        BR_ASSERT((m_scene->mNumMeshes == 2),
                  "Colliding animated object: %s MUST contain 2 meshes. For draw and physics simulation", filePath);

        BR_ASSERT((m_scene->HasAnimations() && (m_scene->mMeshes[0]->mNumBones > 0 || m_scene->mMeshes[1]->mNumBones > 0)),
                  "%s", "Animated object must have animation + bone");

        for(int i = 0; i < m_scene->mNumMeshes; ++i)
        {
            std::string meshName = m_scene->mMeshes[i]->mName.C_Str();

            if(meshName.find("Collision") != std::string::npos)
            {
                // Collect collision mesh dimensions.
                // Model should be created in Blender where up axis = +Z.
                for(int g = 0; g < m_scene->mMeshes[i]->mNumVertices; ++g)
                {
                    // Top and bottom points must be taken from Z axis.
                    if(m_scene->mMeshes[i]->mVertices[g].z < m_mostBottomVertex)
                       m_mostBottomVertex = m_scene->mMeshes[i]->mVertices[g].z;
                    if(m_scene->mMeshes[i]->mVertices[g].z > m_mostTopVertex)
                       m_mostTopVertex = m_scene->mMeshes[i]->mVertices[g].z;

                    if(m_scene->mMeshes[i]->mVertices[g].x < m_smallestX)
                       m_smallestX = m_scene->mMeshes[i]->mVertices[g].x;
                    if(m_scene->mMeshes[i]->mVertices[g].x > m_biggestX)
                       m_biggestX = m_scene->mMeshes[i]->mVertices[g].x;

                    // Z dimensions should be taken from Y axis.
                    // In Blender Y axis is horizontal and will replaced by Z after exporting.
                    if(m_scene->mMeshes[i]->mVertices[g].y < m_smallestZ)
                       m_smallestZ = m_scene->mMeshes[i]->mVertices[g].y;
                    if(m_scene->mMeshes[i]->mVertices[g].y > m_biggestZ)
                       m_biggestZ = m_scene->mMeshes[i]->mVertices[g].y;
                }

                // Colliding object described by collision mesh.
                m_fromOriginToTop = std::abs(m_mostTopVertex);
                m_fromOriginToBottom = std::abs(m_mostBottomVertex);
                m_objectHeight = m_fromOriginToTop + m_fromOriginToBottom;
                m_XZRadius = (std::abs(m_biggestX) + std::abs(m_smallestX)) * 0.5f;
                BR_INFO("m_fromOriginToTop: %f, m_fromOriginToBottom: %f, m_XZRadius: %f, m_objectHeight: %f",
                        m_fromOriginToTop, m_fromOriginToBottom, m_XZRadius, m_objectHeight);

                BR_ASSERT((m_fromOriginToBottom > 0.0f && m_fromOriginToTop > 0.0f && m_XZRadius > 0.0f && m_objectHeight > 0.0f),
                          "%s", "Characters XYZ dimensions are 0.");

                m_collisionFlag = collFlag;
                m_collisionGroup = collGroup;
                m_collisionMask = collMask;
                m_collisionMass = collisionMassKg;

                // Dont add collider to simulation if collGroup == NONE. It have no sense.
                if(collGroup != CollisionGroups::NONE)
                {
                    m_hasCollisionObject = true;
                    m_isEnabledInPhysicsSimulation = true;

                    loadCollisionMesh(m_scene->mMeshes[i], meshName, collisionMassKg, wantCollisionCallBack, collFlag, collGroup, collMask);
                }
            }
        }
    }

    AnimatedCollidingObject::~AnimatedCollidingObject()
    {

    }

    void AnimatedCollidingObject::loadCollisionMesh(const aiMesh* mesh,
                                                    const std::string& meshName,
                                                    float mass,
                                                    bool wantCallBack,
                                                    CollisionFlags collFlag,
                                                    CollisionGroups collGroup,
                                                    CollisionGroups collMask)
    {
        glm::mat4 collisionTransforms{1.0f};

        const aiNode* node = BeryllUtils::Common::findAinodeForAimesh(m_scene, m_scene->mRootNode, mesh->mName);
        if(node)
        {
            collisionTransforms = BeryllUtils::Matrix::aiToGlm(node->mTransformation);
        }

        std::vector<glm::vec3> vertices;
        vertices.reserve(mesh->mNumVertices);
        for(int i = 0; i < mesh->mNumVertices; ++i)
        {
            vertices.emplace_back(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        }

        std::vector<uint32_t> indices;
        indices.reserve(mesh->mNumFaces * 3);
        for(int i = 0; i < mesh->mNumFaces; ++i)
        {
            indices.emplace_back(mesh->mFaces[i].mIndices[0]);
            indices.emplace_back(mesh->mFaces[i].mIndices[1]);
            indices.emplace_back(mesh->mFaces[i].mIndices[2]);
        }

        Physics::addObject(vertices, indices, collisionTransforms, meshName, m_ID, mass, wantCallBack, collFlag, collGroup, collMask);
    }
}
