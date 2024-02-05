#include "SimpleCollidingObject.h"
#include "beryll/utils/File.h"
#include "beryll/utils/Matrix.h"
#include "beryll/utils/CommonUtils.h"
#include "beryll/renderer/Camera.h"
#include "beryll/renderer/Renderer.h"

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
        BR_INFO("Loading colliding simple object: %s", filePath);

        uint32_t bufferSize = 0;
        char* buffer = BeryllUtils::File::readToBuffer(filePath, &bufferSize);

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFileFromMemory(buffer, bufferSize,
                                                           aiProcess_Triangulate |
                                                           aiProcess_FlipUVs |
                                                           aiProcess_CalcTangentSpace);
        delete[] buffer;
        if( !scene || !scene->mRootNode || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE)
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
        disableForEver();
    }

    void SimpleCollidingObject::updateBeforePhysics()
    {

    }

    void SimpleCollidingObject::updateAfterPhysics()
    {
        if(m_collisionFlag != CollisionFlags::DYNAMIC) { return; }

        m_physicsTransforms = Physics::getTransforms(m_ID);

        m_totalRotation = glm::normalize(m_physicsTransforms.rotation);
        m_origin = m_physicsTransforms.origin;
    }

    void SimpleCollidingObject::draw()
    {
        if(useInternalShader)
        {
            m_internalShader->bind();
            m_internalShader->setMatrix4x4Float("MVPMatrix", Camera::getViewProjection() * getModelMatrix());
        }

        if(useInternalMaterials)
        {
            m_material1.bind();

            if(m_material2)
                m_material2->bind();
        }

        m_vertexArray->bind();
        m_vertexArray->draw();
    }

    void SimpleCollidingObject::loadGraphicsMesh(const std::string& filePath, const aiScene* scene, const aiMesh* graphicsMesh)
    {
        BR_INFO("Graphics mesh name: %s", graphicsMesh->mName.C_Str());

        // Prepare vectors.
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec3> tangents;
        std::vector<glm::vec2> textureCoords;
        std::vector<uint32_t> indices;
        vertices.reserve(graphicsMesh->mNumVertices);
        normals.reserve(graphicsMesh->mNumVertices);
        tangents.reserve(graphicsMesh->mNumVertices);
        textureCoords.reserve(graphicsMesh->mNumVertices);
        indices.reserve(graphicsMesh->mNumFaces * 3);

        // Vertices.
        for(int g = 0; g < graphicsMesh->mNumVertices; ++g)
        {
            vertices.emplace_back(graphicsMesh->mVertices[g].x,
                                  graphicsMesh->mVertices[g].y,
                                  graphicsMesh->mVertices[g].z);

            if(graphicsMesh->mNormals)
            {
                glm::vec3 normal = glm::vec3(graphicsMesh->mNormals[g].x,
                                             graphicsMesh->mNormals[g].y,
                                             graphicsMesh->mNormals[g].z);

                normals.emplace_back(glm::normalize(normal));
            }
            else
            {
                normals.emplace_back(0.0f, 0.0f, 0.0f);
            }

            if(graphicsMesh->mTangents)
            {
                glm::vec3 tangent = glm::vec3(graphicsMesh->mTangents[g].x,
                                              graphicsMesh->mTangents[g].y,
                                              graphicsMesh->mTangents[g].z);

                tangents.emplace_back(glm::normalize(tangent));
            }
            else
            {
                tangents.emplace_back(0.0f, 0.0f, 0.0f);
            }

            // Use only first set of texture coordinates.
            if(graphicsMesh->mTextureCoords[0])
            {
                textureCoords.emplace_back(graphicsMesh->mTextureCoords[0][g].x,
                                           graphicsMesh->mTextureCoords[0][g].y);

                if(std::string(graphicsMesh->mName.C_Str()) == "Grid")
                {
                    BR_INFO("textureCoords: %f %f", graphicsMesh->mTextureCoords[0][g].x, graphicsMesh->mTextureCoords[0][g].y);
                }
            }
            else
            {
                textureCoords.emplace_back(0.0f, 0.0f);
            }
        }
        BR_INFO("Vertex count: %d", vertices.size());
        m_vertexPosBuffer = Renderer::createStaticVertexBuffer(vertices);
        m_vertexNormalsBuffer = Renderer::createStaticVertexBuffer(normals);
        m_textureCoordsBuffer = Renderer::createStaticVertexBuffer(textureCoords);
        // Tangents buffer will created if model has normal map.

        // Indices.
        for(int g = 0; g < graphicsMesh->mNumFaces; ++g) // Every face MUST be a triangle !!!!
        {
            indices.emplace_back(graphicsMesh->mFaces[g].mIndices[0]);
            indices.emplace_back(graphicsMesh->mFaces[g].mIndices[1]);
            indices.emplace_back(graphicsMesh->mFaces[g].mIndices[2]);
        }
        BR_INFO("Indices count: %d", indices.size());
        m_indexBuffer = Renderer::createStaticIndexBuffer(indices);

        m_vertexArray = Renderer::createVertexArray();
        m_vertexArray->addVertexBuffer(m_vertexPosBuffer);
        m_vertexArray->addVertexBuffer(m_vertexNormalsBuffer);
        m_vertexArray->addVertexBuffer(m_textureCoordsBuffer);
        // Tangents buffer will added if model has normal map.
        m_vertexArray->setIndexBuffer(m_indexBuffer);

        m_internalShader = Renderer::createShader(BeryllConstants::simpleObjDefaultVertexPath.data(),
                                                  BeryllConstants::simpleObjDefaultFragmentPath.data());
        m_internalShader->bind();

        // Load Material 1. At lest diffuse texture of material 1 must exist.
        if(graphicsMesh->mMaterialIndex >= 0)
        {
            aiMaterial* material = scene->mMaterials[graphicsMesh->mMaterialIndex];

            BR_ASSERT((filePath.find_last_of('/') != std::string::npos), "Texture + model must be in folder: %s", filePath.c_str());

            std::string texturePath;

            if(material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
            {
                aiString textName;
                material->GetTexture(aiTextureType_DIFFUSE, 0, &textName);

                std::string textName2 = textName.C_Str();
                for(int g = static_cast<int>(textName2.size()) - 1; g >= 0; --g)
                {
                    if(textName2[g] == '/' || textName2[g] == '\\')
                    {
                        textName2 = textName2.substr(g + 1);
                        break;
                    }
                }
                texturePath = filePath.substr(0, filePath.find_last_of('/'));
                texturePath += '/';
                texturePath += textName2;
                BR_INFO("Diffuse texture here: %s", texturePath.c_str());

                m_material1.diffTexture = Renderer::createTexture(texturePath.c_str(), TextureType::DIFFUSE_TEXTURE_MAT_1);
                m_internalShader->activateDiffuseTextureMat1();
            }

            if(material->GetTextureCount(aiTextureType_SPECULAR) > 0)
            {
                aiString textName;
                material->GetTexture(aiTextureType_SPECULAR, 0, &textName);

                std::string textName2 = textName.C_Str();
                for(int g = static_cast<int>(textName2.size()) - 1; g >= 0; --g)
                {
                    if(textName2[g] == '/' || textName2[g] == '\\')
                    {
                        textName2 = textName2.substr(g + 1);
                        break;
                    }
                }
                texturePath = filePath.substr(0, filePath.find_last_of('/'));
                texturePath += '/';
                texturePath += textName2;
                BR_INFO("Specular texture here: %s", texturePath.c_str());

                m_material1.specTexture = Renderer::createTexture(texturePath.c_str(), TextureType::SPECULAR_TEXTURE_MAT_1);
                m_internalShader->activateSpecularTextureMat1();
            }

            if(material->GetTextureCount(aiTextureType_NORMALS) > 0)
            {
                aiString textName;
                material->GetTexture(aiTextureType_NORMALS, 0, &textName);

                std::string textName2 = textName.C_Str();
                for(int g = static_cast<int>(textName2.size()) - 1; g >= 0; --g)
                {
                    if(textName2[g] == '/' || textName2[g] == '\\')
                    {
                        textName2 = textName2.substr(g + 1);
                        break;
                    }
                }
                texturePath = filePath.substr(0, filePath.find_last_of('/'));
                texturePath += '/';
                texturePath += textName2;
                BR_INFO("Normal map texture here: %s", texturePath.c_str());

                m_material1.normalMapTexture = Renderer::createTexture(texturePath.c_str(), TextureType::NORMAL_MAP_TEXTURE_MAT_1);
                m_internalShader->activateNormalMapTextureMat1();

                BR_INFO("%s", "Create tangents buffer because model has normal map.");
                m_vertexTangentsBuffer = Renderer::createStaticVertexBuffer(tangents);

                m_vertexArray->addVertexBuffer(m_vertexTangentsBuffer);
            }
        }

        BR_ASSERT((m_material1.diffTexture != nullptr), "%s", "Object must have at least one diffuse texture.");

        const aiNode* node = BeryllUtils::Common::findAinodeForAimesh(scene, scene->mRootNode, graphicsMesh->mName);
        if(node)
        {
            glm::mat4 modelMatrix = BeryllUtils::Matrix::aiToGlm(node->mTransformation);

            glm::vec3 scale = BeryllUtils::Matrix::getScaleFrom4x4Glm(modelMatrix);
            BR_ASSERT((scale.x > 0.9999f && scale.x < 1.0001f &&
                       scale.y > 0.9999f && scale.y < 1.0001f &&
                       scale.z > 0.9999f && scale.z < 1.0001f), "%s", "Scale should be baked to 1 in modeling tool.");

            m_totalRotation = BeryllUtils::Matrix::getRotationFrom4x4Glm(modelMatrix);
            m_origin = BeryllUtils::Matrix::getTranslationFrom4x4Glm(modelMatrix);
        }
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

        m_hasCollisionObject = true;
        m_isEnabledInPhysicsSimulation = true;
        m_collisionFlag = collFlag;
        m_collisionGroup = collGroup;
        m_collisionMask = collMask;
        m_collisionMass = mass;

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
                                                           aiProcess_Triangulate |
                                                           aiProcess_FlipUVs |
                                                           aiProcess_CalcTangentSpace);
        delete[] buffer;
        if( !scene || !scene->mRootNode || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE)
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
