#include "SimpleCollidingObject.h"
#include "beryll/utils/File.h"

namespace Beryll
{
    SimpleCollidingObject::SimpleCollidingObject(const char* filePath,
                                                 float collisionMassKg,
                                                 bool wantCollisionCallBack,
                                                 CollisionFlags collFlag,
                                                 CollisionGroups collGroup,
                                                 CollisionGroups collMask,
                                                 SceneObjectGroups sceneGroup)
    {
        BR_INFO("Loading simple colliding object: %s", filePath);

        uint32_t bufferSize = 0;
        char* buffer = BeryllUtils::File::readToBuffer(filePath, &bufferSize);

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFileFromMemory(buffer, bufferSize,
                                                           aiProcess_Triangulate | aiProcess_FlipUVs |
                                                           aiProcess_JoinIdenticalVertices | aiProcess_CalcTangentSpace);
        delete[] buffer;
        if(!scene || !scene->mRootNode || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE)
        {
            BR_ASSERT(false, "Scene loading error for file: %s", filePath);
        }

        BR_ASSERT((scene->mNumMeshes == 2), "Colliding simple object: %s MUST contain 2 meshes. For draw and physics simulation", filePath);

        m_sceneObjectGroup = sceneGroup;

        for(int i = 0; i < scene->mNumMeshes; ++i)
        {
            std::string meshName = scene->mMeshes[i]->mName.C_Str();

            if(meshName.find("Collision") != std::string::npos)
                loadCollisionMesh(scene, scene->mMeshes[i], meshName, collisionMassKg, wantCollisionCallBack, collFlag, collGroup, collMask);
            else
                loadGraphicsMesh(filePath, scene, scene->mMeshes[i]);
        }
    }

    SimpleCollidingObject::SimpleCollidingObject(const std::string& filePath,
                                                 const aiScene* scene,
                                                 const aiMesh* graphicsMesh,
                                                 const aiMesh* collisionMesh,
                                                 const std::string& collisionMeshName,
                                                 float collisionMassKg,
                                                 bool wantCollisionCallBack,
                                                 CollisionFlags collFlag,
                                                 CollisionGroups collGroup,
                                                 CollisionGroups collMask,
                                                 SceneObjectGroups sceneGroup)
    {
        m_sceneObjectGroup = sceneGroup;

        loadGraphicsMesh(filePath, scene, graphicsMesh);
        loadCollisionMesh(scene, collisionMesh, collisionMeshName, collisionMassKg, wantCollisionCallBack, collFlag, collGroup, collMask);
    }

    SimpleCollidingObject::~SimpleCollidingObject()
    {

    }

    void SimpleCollidingObject::loadCollisionMesh(const aiScene* scene,
                                                  const aiMesh* collisionMesh,
                                                  const std::string& meshName,
                                                  float mass,
                                                  bool wantCallBack,
                                                  CollisionFlags collFlag,
                                                  CollisionGroups collGroup,
                                                  CollisionGroups collMask)
    {
        BR_INFO("Collision mesh name: %s", collisionMesh->mName.C_Str());

        // Collect collision mesh dimensions.
        // Model should be created in Blender where up axis = +Z.
        for (int g = 0; g < collisionMesh->mNumVertices; ++g)
        {
            // Top and bottom points must be taken from Z axis.
            if (collisionMesh->mVertices[g].z < m_mostBottomVertex)
                m_mostBottomVertex = collisionMesh->mVertices[g].z;
            if (collisionMesh->mVertices[g].z > m_mostTopVertex)
                m_mostTopVertex = collisionMesh->mVertices[g].z;

            if (collisionMesh->mVertices[g].x < m_smallestX)
                m_smallestX = collisionMesh->mVertices[g].x;
            if (collisionMesh->mVertices[g].x > m_biggestX)
                m_biggestX = collisionMesh->mVertices[g].x;

            // Z dimensions should be taken from Y axis.
            // In Blender Y axis is horizontal and will replaced by Z after exporting.
            if (collisionMesh->mVertices[g].y < m_smallestZ)
                m_smallestZ = collisionMesh->mVertices[g].y;
            if (collisionMesh->mVertices[g].y > m_biggestZ)
                m_biggestZ = collisionMesh->mVertices[g].y;
        }

        m_collisionFlag = collFlag;
        m_collisionGroup = collGroup;
        m_collisionMask = collMask;
        m_collisionMass = mass;

        // Dont add collider to simulation if collGroup == NONE. It have no sense.
        if(collGroup == CollisionGroups::NONE)
            return;

        m_hasCollisionObject = true;
        m_isEnabledInPhysicsSimulation = true;

        glm::mat4 collisionTransforms{1.0f};

        const aiNode* node = BeryllUtils::Common::findAinodeForAimesh(scene, scene->mRootNode, collisionMesh->mName);
        if(node)
        {
            collisionTransforms = BeryllUtils::Matrix::aiToGlm(node->mTransformation);
        }

        std::vector<glm::vec3> vertices;
        vertices.reserve(collisionMesh->mNumVertices);
        for(int i = 0; i < collisionMesh->mNumVertices; ++i)
        {
            vertices.emplace_back(collisionMesh->mVertices[i].x, collisionMesh->mVertices[i].y, collisionMesh->mVertices[i].z);
        }

        std::vector<uint32_t> indices;
        indices.reserve(collisionMesh->mNumFaces * 3);
        for(int i = 0; i < collisionMesh->mNumFaces; ++i)
        {
            indices.emplace_back(collisionMesh->mFaces[i].mIndices[0]);
            indices.emplace_back(collisionMesh->mFaces[i].mIndices[1]);
            indices.emplace_back(collisionMesh->mFaces[i].mIndices[2]);
        }

        Physics::addObject(vertices, indices, collisionTransforms, meshName, m_ID, mass, wantCallBack, collFlag, collGroup, collMask);
    }

    std::vector<std::shared_ptr<SimpleCollidingObject>> SimpleCollidingObject::loadManyModelsFromOneFile(const char* filePath,
                                                                                                         float collisionMassKg,
                                                                                                         bool wantCollisionCallBack,
                                                                                                         CollisionFlags collFlag,
                                                                                                         CollisionGroups collGroup,
                                                                                                         CollisionGroups collMask,
                                                                                                         SceneObjectGroups sceneGroup)
    {
        std::vector<std::shared_ptr<SimpleCollidingObject>> objects;
        std::shared_ptr<SimpleCollidingObject> obj;

        BR_INFO("Load many colliding simple objects from one file: %s", filePath);

        uint32_t bufferSize = 0;
        char* buffer = BeryllUtils::File::readToBuffer(filePath, &bufferSize);

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFileFromMemory(buffer, bufferSize,
                                                           aiProcess_Triangulate | aiProcess_FlipUVs |
                                                           aiProcess_JoinIdenticalVertices | aiProcess_CalcTangentSpace);
        delete[] buffer;
        if(!scene || !scene->mRootNode || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE)
        {
            BR_ASSERT(false, "Scene loading error for file: %s", filePath);
        }

        BR_INFO("Total mesh count in file: %d", scene->mNumMeshes);

        std::string graphicsMeshName;
        std::string collisionMeshName;

        for(int i = 0; i < scene->mNumMeshes; ++i)
        {
            graphicsMeshName = scene->mMeshes[i]->mName.C_Str();
            if(graphicsMeshName.find("Collision") != std::string::npos)
                continue;

            // Found graphics mesh.
            const aiMesh* graphicsMesh = scene->mMeshes[i];
            const aiMesh* collisionMesh = nullptr;

            // Look for collision mesh for found graphics mesh.
            for(int j = 0; j < scene->mNumMeshes; ++j)
            {
                collisionMeshName = scene->mMeshes[j]->mName.C_Str();

                std::string::size_type collisionWordIndex = collisionMeshName.find("Collision");
                if(collisionWordIndex != std::string::npos)
                {
                    if(graphicsMeshName == collisionMeshName.substr(0, collisionWordIndex))
                    {
                        collisionMesh = scene->mMeshes[j];
                        break;
                    }
                }
            }

            BR_ASSERT((graphicsMesh != nullptr && collisionMesh != nullptr), "Collision mesh not found for graphics mesh: %s", graphicsMeshName.c_str());

            obj = std::make_shared<SimpleCollidingObject>(filePath,
                                                          scene,
                                                          graphicsMesh,
                                                          collisionMesh,
                                                          collisionMeshName,
                                                          collisionMassKg,
                                                          wantCollisionCallBack,
                                                          collFlag,
                                                          collGroup,
                                                          collMask,
                                                          sceneGroup);
            objects.push_back(obj);
        }

        return objects;
    }
}
