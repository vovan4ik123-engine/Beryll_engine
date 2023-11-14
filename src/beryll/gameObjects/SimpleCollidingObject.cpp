#include "SimpleCollidingObject.h"
#include "beryll/utils/File.h"
#include "beryll/utils/Matrix.h"
#include "beryll/utils/CommonUtils.h"
#include "beryll/renderer/Camera.h"
#include "beryll/renderer/Renderer.h"

namespace Beryll
{
    SimpleCollidingObject::SimpleCollidingObject(const char* modelPath,
                                                 float collisionMass,
                                                 bool wantCollisionCallBack,
                                                 CollisionFlags collFlag,
                                                 CollisionGroups collGroup,
                                                 CollisionGroups collMask,
                                                 SceneObjectGroups sceneGroup)
    {
        BR_INFO("Loading colliding simple object: %s", modelPath);

        uint32_t bufferSize = 0;
        char* buffer = Utils::File::readToBuffer(modelPath, &bufferSize);

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFileFromMemory(buffer, bufferSize,
                                                           aiProcess_Triangulate |
                                                           aiProcess_FlipUVs |
                                                           aiProcess_CalcTangentSpace);
        delete[] buffer;
        if( !scene || !scene->mRootNode || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE)
        {
            BR_ASSERT(false, "Scene loading error for file: %s", modelPath);
        }

        BR_ASSERT((scene->mNumMeshes == 2),
                  "Colliding simple object: %s MUST contain 2 meshes. For draw and physics simulation", modelPath);

        m_sceneObjectGroup = sceneGroup;

        for(int i = 0; i < scene->mNumMeshes; ++i)
        {
            std::string meshName = scene->mMeshes[i]->mName.C_Str();

            if(meshName.find("Collision") != std::string::npos)
            {
                // Collect collision mesh dimensions.
                // Model should be created in Blender where up axis = +Z.
                for (int g = 0; g < scene->mMeshes[i]->mNumVertices; ++g)
                {
                    // Top and bottom points must be taken from Z axis.
                    if (scene->mMeshes[i]->mVertices[g].z < m_mostBottomVertex)
                        m_mostBottomVertex = scene->mMeshes[i]->mVertices[g].z;
                    if (scene->mMeshes[i]->mVertices[g].z > m_mostTopVertex)
                        m_mostTopVertex = scene->mMeshes[i]->mVertices[g].z;

                    if (scene->mMeshes[i]->mVertices[g].x < m_smallestX)
                        m_smallestX = scene->mMeshes[i]->mVertices[g].x;
                    if (scene->mMeshes[i]->mVertices[g].x > m_biggestX)
                        m_biggestX = scene->mMeshes[i]->mVertices[g].x;

                    // Z dimensions should be taken from Y axis.
                    // In Blender Y axis is horizontal and will replaced by Z after exporting.
                    if (scene->mMeshes[i]->mVertices[g].y < m_smallestZ)
                        m_smallestZ = scene->mMeshes[i]->mVertices[g].y;
                    if (scene->mMeshes[i]->mVertices[g].y > m_biggestZ)
                        m_biggestZ = scene->mMeshes[i]->mVertices[g].y;
                }

                m_hasCollisionObject = true;
                m_isEnabledInPhysicsSimulation = true;
                m_collisionFlag = collFlag;
                m_collisionGroup = collGroup;
                m_collisionMask = collMask;
                m_collisionMass = collisionMass;

                processCollisionMesh(scene, scene->mMeshes[i], meshName, collisionMass, wantCollisionCallBack, collFlag, collGroup, collMask);
                continue;
            }

            // prepare vectors
            std::vector<glm::vec3> vertices;
            std::vector<glm::vec3> normals;
            std::vector<glm::vec3> tangents;
            std::vector<glm::vec2> textureCoords;
            std::vector<uint32_t> indices;
            vertices.reserve(scene->mMeshes[i]->mNumVertices);
            normals.reserve(scene->mMeshes[i]->mNumVertices);
            tangents.reserve(scene->mMeshes[i]->mNumVertices);
            textureCoords.reserve(scene->mMeshes[i]->mNumVertices);
            indices.reserve(scene->mMeshes[i]->mNumFaces * 3);

            // vertices
            for(int g = 0; g < scene->mMeshes[i]->mNumVertices; ++g)
            {
                vertices.emplace_back(scene->mMeshes[i]->mVertices[g].x,
                                      scene->mMeshes[i]->mVertices[g].y,
                                      scene->mMeshes[i]->mVertices[g].z);

                if(scene->mMeshes[i]->mNormals)
                {
                    glm::vec3 normal = glm::vec3(scene->mMeshes[i]->mNormals[g].x,
                                                 scene->mMeshes[i]->mNormals[g].y,
                                                 scene->mMeshes[i]->mNormals[g].z);

                    normals.emplace_back(glm::normalize(normal));
                }
                else
                {
                    normals.emplace_back(0.0f, 0.0f, 0.0f);
                }

                if(scene->mMeshes[i]->mTangents)
                {
                    glm::vec3 tangent = glm::vec3(scene->mMeshes[i]->mTangents[g].x,
                                                  scene->mMeshes[i]->mTangents[g].y,
                                                  scene->mMeshes[i]->mTangents[g].z);

                    tangents.emplace_back(glm::normalize(tangent));
                }
                else
                {
                    tangents.emplace_back(0.0f, 0.0f, 0.0f);
                }

                // use only first set of texture coordinates
                if(scene->mMeshes[i]->mTextureCoords[0])
                {
                    textureCoords.emplace_back(scene->mMeshes[i]->mTextureCoords[0][g].x,
                                               scene->mMeshes[i]->mTextureCoords[0][g].y);
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
            // tangents buffer will created if model has normal map

            // indices
            for(int g = 0; g < scene->mMeshes[i]->mNumFaces; ++g) // every face MUST be a triangle !!!!
            {
                indices.emplace_back(scene->mMeshes[i]->mFaces[g].mIndices[0]);
                indices.emplace_back(scene->mMeshes[i]->mFaces[g].mIndices[1]);
                indices.emplace_back(scene->mMeshes[i]->mFaces[g].mIndices[2]);
            }
            BR_INFO("Indices count: %d", indices.size());
            m_indexBuffer = Renderer::createStaticIndexBuffer(indices);

            m_vertexArray = Renderer::createVertexArray();
            m_vertexArray->addVertexBuffer(m_vertexPosBuffer);
            m_vertexArray->addVertexBuffer(m_vertexNormalsBuffer);
            m_vertexArray->addVertexBuffer(m_textureCoordsBuffer);
            // tangents buffer will added if model has normal map
            m_vertexArray->setIndexBuffer(m_indexBuffer);

            m_internalShader = Renderer::createShader(BeryllConstants::simpleObjDefaultVertexPath.data(),
                                                      BeryllConstants::simpleObjDefaultFragmentPath.data());
            m_internalShader->bind();

            // material
            if(scene->mMeshes[i]->mMaterialIndex >= 0)
            {
                aiMaterial* material = scene->mMaterials[scene->mMeshes[i]->mMaterialIndex];

                const std::string mP = modelPath;
                BR_ASSERT((mP.find_last_of('/') != std::string::npos), "Texture + model must be in folder: %s", mP.c_str());

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
                    texturePath = mP.substr(0, mP.find_last_of('/'));
                    texturePath += '/';
                    texturePath += textName2;
                    BR_INFO("Diffuse texture here: %s", texturePath.c_str());

                    m_diffTexture = Renderer::createTexture(texturePath.c_str(), TextureType::DIFFUSE_TEXTURE);
                    m_internalShader->activateDiffuseTexture();
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
                    texturePath = mP.substr(0, mP.find_last_of('/'));
                    texturePath += '/';
                    texturePath += textName2;
                    BR_INFO("Specular texture here: %s", texturePath.c_str());

                    m_specTexture = Renderer::createTexture(texturePath.c_str(), TextureType::SPECULAR_TEXTURE);
                    m_internalShader->activateSpecularTexture();
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
                    texturePath = mP.substr(0, mP.find_last_of('/'));
                    texturePath += '/';
                    texturePath += textName2;
                    BR_INFO("Normal map texture here: %s", texturePath.c_str());

                    m_normalMapTexture = Renderer::createTexture(texturePath.c_str(), TextureType::NORMAL_MAP_TEXTURE);
                    m_internalShader->activateNormalMapTexture();

                    BR_INFO("%s", "Create tangents buffer because model has normal map");
                    m_vertexTangentsBuffer = Renderer::createStaticVertexBuffer(tangents);

                    m_vertexArray->addVertexBuffer(m_vertexTangentsBuffer);
                }
            }

            const aiNode* node = Utils::Common::findAinodeForAimesh(scene, scene->mRootNode, scene->mMeshes[i]->mName);
            if(node)
            {
                glm::mat4 modelMatrix = Utils::Matrix::aiToGlm(node->mTransformation);

                glm::vec3 scale = Utils::Matrix::getScaleFrom4x4Glm(modelMatrix);
                BR_ASSERT((scale.x == 1.0f && scale.y == 1.0f && scale.z == 1.0f), "%s", "Scale should be baked to 1 in modeling tool.");

                m_totalRotation = Utils::Matrix::getRotationFrom4x4Glm(modelMatrix);
                m_origin = Utils::Matrix::getTranslationFrom4x4Glm(modelMatrix);
                m_originX = m_origin.x;
                m_originY = m_origin.y;
                m_originZ = m_origin.z;
            }
        }
    }

    SimpleCollidingObject::~SimpleCollidingObject()
    {
        disableCollisionMesh();
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
        m_originX = m_origin.x;
        m_originY = m_origin.y;
        m_originZ = m_origin.z;
    }

    void SimpleCollidingObject::draw()
    {
        if(useInternalShader)
        {
            m_internalShader->bind();
            m_MVP = Camera::getViewProjection() * getModelMatrix();
            m_internalShader->setMatrix4x4Float("MVPMatrix", m_MVP);
        }

        if(m_diffTexture && useInternalTextures) { m_diffTexture->bind(); }
        if(m_specTexture && useInternalTextures) { m_specTexture->bind(); }
        if(m_normalMapTexture && useInternalTextures) { m_normalMapTexture->bind(); }

        m_vertexArray->bind();
        m_vertexArray->draw();
    }

    void SimpleCollidingObject::processCollisionMesh(const aiScene* scene,
                                                     const aiMesh* mesh,
                                                     const std::string& meshName,
                                                     float mass,
                                                     bool wantCallBack,
                                                     CollisionFlags collFlag,
                                                     CollisionGroups collGroup,
                                                     CollisionGroups collMask)
    {
        glm::mat4 collisionTransforms{1.0f};

        const aiNode* node = Utils::Common::findAinodeForAimesh(scene, scene->mRootNode, mesh->mName);
        if(node)
        {
            collisionTransforms = Utils::Matrix::aiToGlm(node->mTransformation);
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
