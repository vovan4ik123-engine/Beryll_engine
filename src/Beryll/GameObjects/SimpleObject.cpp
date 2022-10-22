#include "SimpleObject.h"
#include "Beryll/Core/Log.h"
#include "Beryll/Utils/File.h"
#include "Beryll/Utils/Matrix.h"
#include "Beryll/Utils/CommonUtils.h"
#include "Beryll/Renderer/Camera.h"
#include "Beryll/Renderer/Renderer.h"
#include "Beryll/Core/Window.h"

namespace Beryll
{
    SimpleObject::SimpleObject(const char* modelPath)
    {
        BR_INFO("Loading simple object:%s", modelPath);

        uint32_t bufferSize = 0;
        char* buffer = Utils::File::readToBuffer(modelPath, &bufferSize);

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFileFromMemory(buffer, bufferSize,
                                                           aiProcess_Triangulate |
                                                            aiProcess_SortByPType |
                                                            aiProcess_FlipUVs);
        delete[] buffer;
        if( !scene || !scene->mRootNode || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE)
        {
            BR_ASSERT(false, "Scene loading error for file:%s", modelPath);
        }

        BR_ASSERT((scene->mNumMeshes == 1),
                "Simple object:%s MUST contain only 1 mesh. Combine into one if you have many", modelPath);

        // prepare vectors
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> textureCoords;
        std::vector<uint32_t> indices;
        vertices.reserve(scene->mMeshes[0]->mNumVertices);
        normals.reserve(scene->mMeshes[0]->mNumVertices);
        textureCoords.reserve(scene->mMeshes[0]->mNumVertices);
        indices.reserve(scene->mMeshes[0]->mNumFaces * 3);

        // vertices
        for(int i = 0; i < scene->mMeshes[0]->mNumVertices; ++i)
        {
            vertices.emplace_back(scene->mMeshes[0]->mVertices[i].x,
                                  scene->mMeshes[0]->mVertices[i].y,
                                  scene->mMeshes[0]->mVertices[i].z);

            if(scene->mMeshes[0]->mNormals)
            {
                glm::vec3 normal = glm::vec3(scene->mMeshes[0]->mNormals[i].x,
                                             scene->mMeshes[0]->mNormals[i].y,
                                             scene->mMeshes[0]->mNormals[i].z);

                normals.emplace_back(glm::normalize(normal));
            }
            else
            {
                normals.emplace_back(0.0f, 0.0f, 0.0f);
            }

            // use only first set of texture coordinates
            if(scene->mMeshes[0]->mTextureCoords[0])
            {
                textureCoords.emplace_back(scene->mMeshes[0]->mTextureCoords[0][i].x,
                                           scene->mMeshes[0]->mTextureCoords[0][i].y);
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
        for(int i = 0; i < scene->mMeshes[0]->mNumFaces; ++i) // every face MUST be a triangle !!!!
        {
            indices.emplace_back(scene->mMeshes[0]->mFaces[i].mIndices[0]);
            indices.emplace_back(scene->mMeshes[0]->mFaces[i].mIndices[1]);
            indices.emplace_back(scene->mMeshes[0]->mFaces[i].mIndices[2]);
        }
        m_indexBuffer = Renderer::createIndexBuffer(indices);

        m_vertexArray = Renderer::createVertexArray();
        m_vertexArray->addVertexBuffer(m_vertexPosBuffer);
        m_vertexArray->addVertexBuffer(m_vertexNormalsBuffer);
        m_vertexArray->addVertexBuffer(m_textureCoordsBuffer);
        m_vertexArray->setIndexBuffer(m_indexBuffer);

        m_internalShader = Renderer::createShader("shaders/GLES/default/Simple.vert", "shaders/GLES/default/Simple.frag");
        m_internalShader->bind();

        // material
        if(scene->mMeshes[0]->mMaterialIndex >= 0)
        {
            aiMaterial* material = scene->mMaterials[scene->mMeshes[0]->mMaterialIndex];

            const std::string mP = modelPath;
            BR_ASSERT((mP.find_last_of('/') != std::string::npos), "Texture + model must be in folder:%s", mP.c_str());

            std::string texturePath;

            if(material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
            {
                aiString textName;
                material->GetTexture(aiTextureType_DIFFUSE, 0, &textName);

                texturePath = mP.substr(0, mP.find_last_of('/'));
                texturePath += '/';
                texturePath += textName.C_Str();
                BR_INFO("Diffuse texture here:%s", texturePath.c_str());

                m_diffTexture = Renderer::createTexture(texturePath.c_str(), TextureType::DIFFUSE_TEXTURE);
                m_internalShader->activateDiffuseTexture();
            }

            if(material->GetTextureCount(aiTextureType_SPECULAR) > 0)
            {
                aiString textName;
                material->GetTexture(aiTextureType_SPECULAR, 0, &textName);

                texturePath = mP.substr(0, mP.find_last_of('/'));
                texturePath += '/';
                texturePath += textName.C_Str();
                BR_INFO("Specular texture here:%s", texturePath.c_str());

                m_specTexture = Renderer::createTexture(texturePath.c_str(), TextureType::SPECULAR_TEXTURE);
                m_internalShader->activateSpecularTexture();
            }
        }

        const aiNode* node = Utils::Common::findAinodeForAimesh(scene, scene->mRootNode, scene->mMeshes[0]->mName);
        if(node)
        {
            m_modelMatrix = Utils::Matrix::aiToGlm(node->mTransformation);
        }

        m_origin = Utils::Matrix::getTranslationFrom4x4Glm(m_modelMatrix);
    }

    SimpleObject::~SimpleObject()
    {

    }

    void SimpleObject::updateBeforePhysics()
    {

    }

    void SimpleObject::updateAfterPhysics()
    {

    }

    void SimpleObject::draw()
    {
        if(useInternalShader)
        {
            m_internalShader->bind();
            m_MVP = Camera::getViewProjection() * m_modelMatrix;
            m_internalShader->setMatrix4x4Float("MVPMatrix", m_MVP);
        }

        if(m_diffTexture && useInternalTextures) { m_diffTexture->bind(); }
        if(m_specTexture && useInternalTextures) { m_specTexture->bind(); }

        m_vertexArray->bind();
        m_vertexArray->draw();
    }

    void SimpleObject::playSound()
    {

    }
}