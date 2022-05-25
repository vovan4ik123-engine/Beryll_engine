#include "SimpleObject.h"
#include "Beryll/Core/Log.h"
#include "Beryll/Utils/File.h"
#include "Beryll/Utils/Matrix.h"
#include "Beryll/Renderer/Camera.h"
#include "Beryll/Core/Window.h"

namespace Beryll
{
    SimpleObject::SimpleObject(const char* modelPath,
                               bool canBeDisabled,
                               const char* vertexPath,
                               const char* fragmentPath,
                               const char* diffSampler,
                               const char* specSampler)
    {
        BR_INFO("Loading simple object:{0}", modelPath);

        uint32_t bufferSize = 0;
        char* buffer = File::readToBuffer(modelPath, &bufferSize);

        m_scene = m_importer.ReadFileFromMemory(buffer, bufferSize,
                                                aiProcess_Triangulate |
                                                        aiProcess_SortByPType |
                                                        aiProcess_FlipUVs);

        if( !m_scene || !m_scene->mRootNode || m_scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE)
        {
            BR_ASSERT(false, "Scene loading error for file {0}", modelPath);
        }

        BR_ASSERT((m_scene->mNumMeshes == 1),
                "Simple object MUST contain only 1 mesh {0}. Combine into one if you have many", modelPath);

        m_ID = "SimpleObject_" + std::to_string(m_allSimpleObjectCount);
        ++m_allSimpleObjectCount;

        m_canBeDisabled = canBeDisabled;

        // prepare vectors
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> textureCoords;
        std::vector<uint32_t> indices;
        vertices.reserve(m_scene->mMeshes[0]->mNumVertices);
        normals.reserve(m_scene->mMeshes[0]->mNumVertices);
        textureCoords.reserve(m_scene->mMeshes[0]->mNumVertices);
        indices.reserve(m_scene->mMeshes[0]->mNumFaces * 3);

        // vertices
        for(int g = 0; g < m_scene->mMeshes[0]->mNumVertices; ++g)
        {
            vertices.emplace_back(m_scene->mMeshes[0]->mVertices[g].x,
                                    m_scene->mMeshes[0]->mVertices[g].y,
                                    m_scene->mMeshes[0]->mVertices[g].z);

            if(m_scene->mMeshes[0]->mNormals)
            {
                normals.emplace_back(m_scene->mMeshes[0]->mNormals[g].x,
                                       m_scene->mMeshes[0]->mNormals[g].y,
                                       m_scene->mMeshes[0]->mNormals[g].z);
            }
            else
            {
                normals.emplace_back(0.0f, 0.0f, 0.0f);
            }

            // use only first set of texture coordinates
            if(m_scene->mMeshes[0]->mTextureCoords[0])
            {
                textureCoords.emplace_back(m_scene->mMeshes[0]->mTextureCoords[0][g].x,
                                             m_scene->mMeshes[0]->mTextureCoords[0][g].y);
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
        for(int g = 0; g < m_scene->mMeshes[0]->mNumFaces; ++g) // every face MUST be a triangle !!!!
        {
            indices.emplace_back(m_scene->mMeshes[0]->mFaces[g].mIndices[0]);
            indices.emplace_back(m_scene->mMeshes[0]->mFaces[g].mIndices[1]);
            indices.emplace_back(m_scene->mMeshes[0]->mFaces[g].mIndices[2]);
        }
        m_indexBuffer = Renderer::createIndexBuffer(indices);

        m_vertexArray = Renderer::createVertexArray();
        m_vertexArray->addVertexBuffer(m_vertexPosBuffer);
        m_vertexArray->addVertexBuffer(m_vertexNormalsBuffer);
        m_vertexArray->addVertexBuffer(m_textureCoordsBuffer);
        m_vertexArray->setIndexBuffer(m_indexBuffer);

        m_shader = Renderer::createShader(vertexPath, fragmentPath);

        // material
        if(m_scene->mMeshes[0]->mMaterialIndex >= 0)
        {
            aiMaterial* material = m_scene->mMaterials[m_scene->mMeshes[0]->mMaterialIndex];

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

        aiNode* node = m_scene->mRootNode->FindNode(m_scene->mMeshes[0]->mName);
        if(node)
        {
            m_modelMatrix = Matrix::aiToGlm(node->mTransformation);
        }

        m_position = Matrix::getPositionFrom4x4Glm(m_modelMatrix);
    }

    SimpleObject::~SimpleObject()
    {

    }

    uint32_t SimpleObject::m_allSimpleObjectCount = 0;

    void SimpleObject::updateBeforePhysics()
    {

    }

    void SimpleObject::updateAfterPhysics()
    {

    }

    void SimpleObject::draw()
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

    void SimpleObject::playSound()
    {

    }
}