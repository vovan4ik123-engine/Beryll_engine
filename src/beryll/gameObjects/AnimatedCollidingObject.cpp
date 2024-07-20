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
                loadCollisionMesh(m_scene->mMeshes[i], meshName, collisionMassKg, wantCollisionCallBack, collFlag, collGroup, collMask);
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
        // Collect collision mesh dimensions.
        // Model should be created in Blender where up axis = +Z.
        for(int g = 0; g < mesh->mNumVertices; ++g)
        {
            // Top and bottom points must be taken from Z axis.
            if(mesh->mVertices[g].z < m_mostBottomVertex)
                m_mostBottomVertex = mesh->mVertices[g].z;
            if(mesh->mVertices[g].z > m_mostTopVertex)
                m_mostTopVertex = mesh->mVertices[g].z;

            if(mesh->mVertices[g].x < m_smallestX)
                m_smallestX = mesh->mVertices[g].x;
            if(mesh->mVertices[g].x > m_biggestX)
                m_biggestX = mesh->mVertices[g].x;

            // Z dimensions should be taken from Y axis.
            // In Blender Y axis is horizontal and will replaced by Z after exporting.
            if(mesh->mVertices[g].y < m_smallestZ)
                m_smallestZ = mesh->mVertices[g].y;
            if(mesh->mVertices[g].y > m_biggestZ)
                m_biggestZ = mesh->mVertices[g].y;
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
        m_collisionMass = mass;

        glm::mat4 collisionTransforms{1.0f};

        const aiNode* node = BeryllUtils::Common::findAinodeForAimesh(m_scene, m_scene->mRootNode, mesh->mName);
        if(node)
        {
            collisionTransforms = BeryllUtils::Matrix::aiToGlm(node->mTransformation);
        }
        // Check scale. Should be 1.
        glm::vec3 scale = BeryllUtils::Matrix::getScaleFrom4x4Glm(collisionTransforms);
        BR_ASSERT((scale.x > 0.9999f && scale.x < 1.0001f &&
                   scale.y > 0.9999f && scale.y < 1.0001f &&
                   scale.z > 0.9999f && scale.z < 1.0001f), "%s", "Scale should be baked to 1 in modeling tool.");


        // Dont add collider to simulation if collGroup == NONE. It have no sense.
        if(collGroup == CollisionGroups::NONE)
            return;

        m_hasCollisionObject = true;
        m_isEnabledInPhysicsSimulation = true;

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
