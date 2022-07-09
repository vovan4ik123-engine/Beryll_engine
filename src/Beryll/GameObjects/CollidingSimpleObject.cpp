#include "CollidingSimpleObject.h"
#include "Beryll/Core/Log.h"
#include "Beryll/Utils/File.h"
#include "Beryll/Utils/Matrix.h"
#include "Beryll/Utils/CommonUtils.h"
#include "Beryll/Core/Window.h"
#include "Beryll/Renderer/Camera.h"

namespace Beryll
{
    CollidingSimpleObject::CollidingSimpleObject(const char* modelPath,
                                                 bool canBeDisabled,
                                                 float collisionMass,
                                                 bool wantCollisionCallBack,
                                                 CollisionFlags collFlag,
                                                 CollisionGroups collGroup,
                                                 CollisionGroups collMask,
                                                 const char* vertexPath,
                                                 const char* fragmentPath,
                                                 const char* diffSampler,
                                                 const char* specSampler)
    {
        BR_INFO("Loading colliding simple object:{0}", modelPath);

        uint32_t bufferSize = 0;
        char* buffer = Utils::File::readToBuffer(modelPath, &bufferSize);

        m_scene = m_importer.ReadFileFromMemory(buffer, bufferSize,
                                                aiProcess_Triangulate |
                                                aiProcess_SortByPType |
                                                aiProcess_FlipUVs);

        if( !m_scene || !m_scene->mRootNode || m_scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE)
        {
            BR_ASSERT(false, "Scene loading error for file {0}", modelPath);
        }

        BR_ASSERT((m_scene->mNumMeshes == 2),
                  "Colliding simple object MUST contain 2 meshes {0}. For draw and physics simulation", modelPath);

        m_canBeDisabled = canBeDisabled;

        for(int i = 0; i < m_scene->mNumMeshes; ++i)
        {
            std::string meshName = m_scene->mMeshes[i]->mName.C_Str();

            if(meshName.find("Collision") != std::string::npos)
            {
                m_hasCollisionObject = true;
                processCollisionMesh(m_scene->mMeshes[i], meshName, collisionMass, wantCollisionCallBack, collFlag, collGroup, collMask);
                continue;
            }

            // prepare vectors
            std::vector<glm::vec3> vertices;
            std::vector<glm::vec3> normals;
            std::vector<glm::vec2> textureCoords;
            std::vector<uint32_t> indices;
            vertices.reserve(m_scene->mMeshes[i]->mNumVertices);
            normals.reserve(m_scene->mMeshes[i]->mNumVertices);
            textureCoords.reserve(m_scene->mMeshes[i]->mNumVertices);
            indices.reserve(m_scene->mMeshes[i]->mNumFaces * 3);

            // vertices
            for(int g = 0; g < m_scene->mMeshes[i]->mNumVertices; ++g)
            {
                vertices.emplace_back(m_scene->mMeshes[i]->mVertices[g].x,
                                      m_scene->mMeshes[i]->mVertices[g].y,
                                      m_scene->mMeshes[i]->mVertices[g].z);

                if(m_scene->mMeshes[i]->mNormals)
                {
                    normals.emplace_back(m_scene->mMeshes[i]->mNormals[g].x,
                                         m_scene->mMeshes[i]->mNormals[g].y,
                                         m_scene->mMeshes[i]->mNormals[g].z);
                }
                else
                {
                    normals.emplace_back(0.0f, 0.0f, 0.0f);
                }

                // use only first set of texture coordinates
                if(m_scene->mMeshes[i]->mTextureCoords[0])
                {
                    textureCoords.emplace_back(m_scene->mMeshes[i]->mTextureCoords[0][g].x,
                                               m_scene->mMeshes[i]->mTextureCoords[0][g].y);
                }
                else
                {
                    textureCoords.emplace_back(0.0f, 0.0f);
                }
            }
            m_vertexPosBuffer = Renderer::createVertexBuffer(vertices);
            m_vertexNormalsBuffer = Renderer::createVertexBuffer(normals);
            m_textureCoordsBuffer = Renderer::createVertexBuffer(textureCoords);

            // indices
            for(int g = 0; g < m_scene->mMeshes[i]->mNumFaces; ++g) // every face MUST be a triangle !!!!
            {
                indices.emplace_back(m_scene->mMeshes[i]->mFaces[g].mIndices[0]);
                indices.emplace_back(m_scene->mMeshes[i]->mFaces[g].mIndices[1]);
                indices.emplace_back(m_scene->mMeshes[i]->mFaces[g].mIndices[2]);
            }
            m_indexBuffer = Renderer::createIndexBuffer(indices);

            m_vertexArray = Renderer::createVertexArray();
            m_vertexArray->addVertexBuffer(m_vertexPosBuffer);
            m_vertexArray->addVertexBuffer(m_vertexNormalsBuffer);
            m_vertexArray->addVertexBuffer(m_textureCoordsBuffer);
            m_vertexArray->setIndexBuffer(m_indexBuffer);

            m_shader = Renderer::createShader(vertexPath, fragmentPath);

            // material
            if(m_scene->mMeshes[i]->mMaterialIndex >= 0)
            {
                aiMaterial* material = m_scene->mMaterials[m_scene->mMeshes[i]->mMaterialIndex];

                const std::string mP = modelPath;
                BR_ASSERT((mP.find_last_of('/') != std::string::npos), "Texture + model must be in folder:{0}", mP);

                std::string texturePath;

                if(material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
                {
                    aiString textName;
                    material->GetTexture(aiTextureType_DIFFUSE, 0, &textName);

                    texturePath = mP.substr(0, mP.find_last_of('/'));
                    texturePath += '/';
                    texturePath += textName.C_Str();
                    BR_INFO("Diffuse texture here:{0}", texturePath);

                    m_shader->activateTexture(diffSampler, m_diffSamplerIndexInShader);

                    m_diffTexture = Renderer::createTexture(texturePath.c_str(), m_diffSamplerIndexInShader);
                }

                if(material->GetTextureCount(aiTextureType_SPECULAR) > 0 && specSampler != nullptr)
                {
                    aiString textName;
                    material->GetTexture(aiTextureType_SPECULAR, 0, &textName);

                    texturePath = mP.substr(0, mP.find_last_of('/'));
                    texturePath += '/';
                    texturePath += textName.C_Str();
                    BR_INFO("Specular texture here:{0}", texturePath);

                    m_shader->activateTexture(specSampler, m_specSamplerIndexInShader);

                    m_specTexture = Renderer::createTexture(texturePath.c_str(), m_specSamplerIndexInShader);
                }
            }

            const aiNode* node = Utils::Common::findAinodeForAimesh(m_scene, m_scene->mRootNode, m_scene->mMeshes[i]->mName);
            if(node)
            {
                m_modelMatrix = Utils::Matrix::aiToGlm(node->mTransformation);
            }

            m_scaleMatrix = glm::scale(glm::mat4x4{1.0f}, Utils::Matrix::getScaleFrom4x4Glm(m_modelMatrix));
            m_origin = Utils::Matrix::getPositionFrom4x4Glm(m_modelMatrix);
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

        m_translateMatrix = glm::translate(glm::mat4x4{1.0f}, m_physicsTransforms.origin);
        m_rotateMatrix = glm::toMat4(m_physicsTransforms.rotation);

        m_modelMatrix = m_translateMatrix * m_rotateMatrix * m_scaleMatrix;
    }

    void CollidingSimpleObject::draw()
    {
        m_MVP = Camera::get3DCamera() * m_modelMatrix;

        m_shader->bind();
        m_shader->setMatrix4x4Float("MVP_matrix", m_MVP);

        if(m_diffTexture) { m_diffTexture->bind(); }
        if(m_specTexture) { m_specTexture->bind(); }

        m_vertexArray->bind();
        m_vertexArray->draw();
        m_vertexArray->unBind();

        if(m_diffTexture) { m_diffTexture->unBind(); }
        if(m_specTexture) { m_specTexture->unBind(); }

        m_shader->unBind();
    }

    void CollidingSimpleObject::playSound()
    {

    }

    void CollidingSimpleObject::processCollisionMesh(const aiMesh* mesh,
                                                     const std::string& meshName,
                                                     float mass,
                                                     bool wantCallBack,
                                                     CollisionFlags collFlag,
                                                     CollisionGroups collGroup,
                                                     CollisionGroups collMask)
    {
        glm::mat4 collisionTransforms{1.0f};

        const aiNode* node = Utils::Common::findAinodeForAimesh(m_scene, m_scene->mRootNode, mesh->mName);
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