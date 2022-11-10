#include "CollidingSimpleObject.h"
#include "Beryll/Core/Log.h"
#include "Beryll/Utils/File.h"
#include "Beryll/Utils/Matrix.h"
#include "Beryll/Utils/CommonUtils.h"
#include "Beryll/Core/Window.h"
#include "Beryll/Renderer/Camera.h"
#include "Beryll/Renderer/Renderer.h"

namespace Beryll
{
    CollidingSimpleObject::CollidingSimpleObject(const char* modelPath,
                                                 float collisionMass,
                                                 bool wantCollisionCallBack,
                                                 CollisionFlags collFlag,
                                                 CollisionGroups collGroup,
                                                 CollisionGroups collMask)
    {
        BR_INFO("Loading colliding simple object:%s", modelPath);

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
            BR_ASSERT(false, "Scene loading error for file:%s", modelPath);
        }

        BR_ASSERT((scene->mNumMeshes == 2),
                  "Colliding simple object:%s MUST contain 2 meshes. For draw and physics simulation", modelPath);

        for(int i = 0; i < scene->mNumMeshes; ++i)
        {
            std::string meshName = scene->mMeshes[i]->mName.C_Str();
            BR_INFO("mesh name %s", scene->mMeshes[i]->mName.C_Str());

            if(meshName.find("Collision") != std::string::npos)
            {
                // collect collision mesh dimensions
                for (int g = 0; g < scene->mMeshes[i]->mNumVertices; ++g)
                {
                    if (scene->mMeshes[i]->mVertices[g].y < m_mostBottomVertex)
                        m_mostBottomVertex = scene->mMeshes[i]->mVertices[g].y;

                    if (scene->mMeshes[i]->mVertices[g].y > m_mostTopVertex)
                        m_mostTopVertex = scene->mMeshes[i]->mVertices[g].y;

                    if (scene->mMeshes[i]->mVertices[g].x < m_smallestX)
                        m_smallestX = scene->mMeshes[i]->mVertices[g].x;

                    if (scene->mMeshes[i]->mVertices[g].x > m_biggestX)
                        m_biggestX = scene->mMeshes[i]->mVertices[g].x;

                    if (scene->mMeshes[i]->mVertices[g].z < m_smallestZ)
                        m_smallestZ = scene->mMeshes[i]->mVertices[g].z;

                    if (scene->mMeshes[i]->mVertices[g].z > m_biggestZ)
                        m_biggestZ = scene->mMeshes[i]->mVertices[g].z;
                }

                m_hasCollisionObject = true;
                m_isEnabledInPhysicsSimulation = true;
                m_collisionGroup = collGroup;

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
            m_vertexPosBuffer = Renderer::createVertexBuffer(vertices);
            m_vertexNormalsBuffer = Renderer::createVertexBuffer(normals);
            m_vertexTangentsBuffer = Renderer::createVertexBuffer(tangents);
            m_textureCoordsBuffer = Renderer::createVertexBuffer(textureCoords);

            // indices
            for(int g = 0; g < scene->mMeshes[i]->mNumFaces; ++g) // every face MUST be a triangle !!!!
            {
                indices.emplace_back(scene->mMeshes[i]->mFaces[g].mIndices[0]);
                indices.emplace_back(scene->mMeshes[i]->mFaces[g].mIndices[1]);
                indices.emplace_back(scene->mMeshes[i]->mFaces[g].mIndices[2]);
            }
            m_indexBuffer = Renderer::createIndexBuffer(indices);

            m_vertexArray = Renderer::createVertexArray();
            m_vertexArray->addVertexBuffer(m_vertexPosBuffer);
            m_vertexArray->addVertexBuffer(m_vertexNormalsBuffer);
            m_vertexArray->addVertexBuffer(m_textureCoordsBuffer);
            m_vertexArray->addVertexBuffer(m_vertexTangentsBuffer);
            m_vertexArray->setIndexBuffer(m_indexBuffer);

            m_internalShader = Renderer::createShader("shaders/GLES/default/Simple.vert", "shaders/GLES/default/Simple.frag");
            m_internalShader->bind();

            // material
            if(scene->mMeshes[i]->mMaterialIndex >= 0)
            {
                aiMaterial* material = scene->mMaterials[scene->mMeshes[i]->mMaterialIndex];

                const std::string mP = modelPath;
                BR_ASSERT((mP.find_last_of('/') != std::string::npos), "Texture + model must be in folder:%s", mP.c_str());

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
                    BR_INFO("Diffuse texture here:%s", texturePath.c_str());

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
                    BR_INFO("Specular texture here:%s", texturePath.c_str());

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
                    BR_INFO("Normal map texture here:%s", texturePath.c_str());

                    m_normalMapTexture = Renderer::createTexture(texturePath.c_str(), TextureType::NORMAL_MAP_TEXTURE);
                    m_internalShader->activateNormalMapTexture();
                }
            }

            const aiNode* node = Utils::Common::findAinodeForAimesh(scene, scene->mRootNode, scene->mMeshes[i]->mName);
            if(node)
            {
                m_modelMatrix = Utils::Matrix::aiToGlm(node->mTransformation);
            }

            m_scaleMatrix = glm::scale(glm::mat4{1.0f}, Utils::Matrix::getScaleFrom4x4Glm(m_modelMatrix));
            m_origin = Utils::Matrix::getTranslationFrom4x4Glm(m_modelMatrix);
        }
    }

    CollidingSimpleObject::~CollidingSimpleObject()
    {

    }

    void CollidingSimpleObject::updateBeforePhysics()
    {

    }

    void CollidingSimpleObject::updateAfterPhysics()
    {
        m_physicsTransforms = Physics::getTransforms(m_ID);

        m_origin = m_physicsTransforms.origin;

        m_translateMatrix = glm::translate(glm::mat4{1.0f}, m_physicsTransforms.origin);
        m_rotateMatrix = glm::toMat4(m_physicsTransforms.rotation);

        m_modelMatrix = m_translateMatrix * m_rotateMatrix * m_scaleMatrix;
    }

    void CollidingSimpleObject::draw()
    {
        if(useInternalShader)
        {
            m_internalShader->bind();
            m_MVP = Camera::getViewProjection() * m_modelMatrix;
            m_internalShader->setMatrix4x4Float("MVPMatrix", m_MVP);
        }

        if(m_diffTexture && useInternalTextures) { m_diffTexture->bind(); }
        if(m_specTexture && useInternalTextures) { m_specTexture->bind(); }
        if(m_normalMapTexture && useInternalTextures) { m_normalMapTexture->bind(); }

        m_vertexArray->bind();
        m_vertexArray->draw();
    }

    void CollidingSimpleObject::playSound()
    {

    }

    void CollidingSimpleObject::processCollisionMesh(const aiScene* scene,
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
