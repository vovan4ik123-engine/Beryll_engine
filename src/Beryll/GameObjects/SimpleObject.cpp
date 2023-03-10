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
    SimpleObject::SimpleObject(const char* modelPath,
                               SceneObjectGroups objGroup)
    {
        BR_INFO("Loading simple object: %s", modelPath);

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

        BR_ASSERT((scene->mNumMeshes == 1),
                "Simple object: %s MUST contain only 1 mesh. Combine into one if you have many", modelPath);

        m_sceneObjectGroup = objGroup;

        // prepare vectors
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec3> tangents;
        std::vector<glm::vec2> textureCoords;
        std::vector<uint32_t> indices;
        vertices.reserve(scene->mMeshes[0]->mNumVertices);
        normals.reserve(scene->mMeshes[0]->mNumVertices);
        tangents.reserve(scene->mMeshes[0]->mNumVertices);
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

            if(scene->mMeshes[0]->mTangents)
            {
                glm::vec3 tangent = glm::vec3(scene->mMeshes[0]->mTangents[i].x,
                                              scene->mMeshes[0]->mTangents[i].y,
                                              scene->mMeshes[0]->mTangents[i].z);

                tangents.emplace_back(glm::normalize(tangent));
            }
            else
            {
                tangents.emplace_back(0.0f, 0.0f, 0.0f);
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
        BR_INFO("Vertex count: %d", vertices.size());
        m_vertexPosBuffer = Renderer::createStaticVertexBuffer(vertices);
        m_vertexNormalsBuffer = Renderer::createStaticVertexBuffer(normals);
        m_vertexTangentsBuffer = Renderer::createStaticVertexBuffer(tangents);
        m_textureCoordsBuffer = Renderer::createStaticVertexBuffer(textureCoords);

        // indices
        for(int i = 0; i < scene->mMeshes[0]->mNumFaces; ++i) // every face MUST be a triangle !!!!
        {
            indices.emplace_back(scene->mMeshes[0]->mFaces[i].mIndices[0]);
            indices.emplace_back(scene->mMeshes[0]->mFaces[i].mIndices[1]);
            indices.emplace_back(scene->mMeshes[0]->mFaces[i].mIndices[2]);
        }
        m_indexBuffer = Renderer::createStaticIndexBuffer(indices);

        m_vertexArray = Renderer::createVertexArray();
        m_vertexArray->addVertexBuffer(m_vertexPosBuffer);
        m_vertexArray->addVertexBuffer(m_vertexNormalsBuffer);
        m_vertexArray->addVertexBuffer(m_textureCoordsBuffer);
        m_vertexArray->addVertexBuffer(m_vertexTangentsBuffer);
        m_vertexArray->setIndexBuffer(m_indexBuffer);

        m_internalShader = Renderer::createShader(BeryllConstants::simpleObjDefaultVertexPath.data(),
                                                  BeryllConstants::simpleObjDefaultFragmentPath.data());
        m_internalShader->bind();

        // material
        if(scene->mMeshes[0]->mMaterialIndex >= 0)
        {
            aiMaterial* material = scene->mMaterials[scene->mMeshes[0]->mMaterialIndex];

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
            }
        }

        const aiNode* node = Utils::Common::findAinodeForAimesh(scene, scene->mRootNode, scene->mMeshes[0]->mName);
        if(node)
        {
            m_modelMatrix = Utils::Matrix::aiToGlm(node->mTransformation);

            m_scaleMatrix = glm::scale(glm::mat4{1.0f}, Utils::Matrix::getScaleFrom4x4Glm(m_modelMatrix));
            m_rotateMatrix = glm::toMat4(Utils::Matrix::getRotationFrom4x4Glm(m_modelMatrix));
            m_origin = Utils::Matrix::getTranslationFrom4x4Glm(m_modelMatrix);
            m_translateMatrix = glm::translate(glm::mat4{1.0f}, m_origin);
        }
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
        if(m_normalMapTexture && useInternalTextures) { m_normalMapTexture->bind(); }

        m_vertexArray->bind();
        m_vertexArray->draw();
    }

    void SimpleObject::playSound()
    {

    }
}
