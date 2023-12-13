#include "SimpleObject.h"
#include "beryll/utils/File.h"
#include "beryll/utils/Matrix.h"
#include "beryll/utils/CommonUtils.h"
#include "beryll/renderer/Camera.h"
#include "beryll/renderer/Renderer.h"

namespace Beryll
{
    SimpleObject::SimpleObject(const char* filePath,
                               SceneObjectGroups sceneGroup)
    {
        BR_INFO("Loading simple object: %s", filePath);

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

        BR_ASSERT((scene->mNumMeshes == 1), "Simple object: %s MUST contain only 1 mesh.", filePath);

        m_sceneObjectGroup = sceneGroup;

        loadGraphicsMesh(filePath, scene, scene->mMeshes[0]);
    }

    SimpleObject::SimpleObject(const std::string& filePath,
                               const aiScene* scene,
                               const aiMesh* graphicsMesh,
                               SceneObjectGroups sceneGroup)
    {
        m_sceneObjectGroup = sceneGroup;

        loadGraphicsMesh(filePath, scene, graphicsMesh);
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
            m_internalShader->setMatrix4x4Float("MVPMatrix", Camera::getViewProjection() * getModelMatrix());
        }

        if(m_diffTexture && useInternalTextures) { m_diffTexture->bind(); }
        if(m_specTexture && useInternalTextures) { m_specTexture->bind(); }
        if(m_normalMapTexture && useInternalTextures) { m_normalMapTexture->bind(); }

        m_vertexArray->bind();
        m_vertexArray->draw();
    }

    void SimpleObject::loadGraphicsMesh(const std::string& filePath, const aiScene* scene, const aiMesh* graphicsMesh)
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
        for(int i = 0; i < graphicsMesh->mNumVertices; ++i)
        {
            vertices.emplace_back(graphicsMesh->mVertices[i].x,
                                  graphicsMesh->mVertices[i].y,
                                  graphicsMesh->mVertices[i].z);

            if(graphicsMesh->mNormals)
            {
                glm::vec3 normal = glm::vec3(graphicsMesh->mNormals[i].x,
                                             graphicsMesh->mNormals[i].y,
                                             graphicsMesh->mNormals[i].z);

                normals.emplace_back(glm::normalize(normal));
            }
            else
            {
                normals.emplace_back(0.0f, 0.0f, 0.0f);
            }

            if(graphicsMesh->mTangents)
            {
                glm::vec3 tangent = glm::vec3(graphicsMesh->mTangents[i].x,
                                              graphicsMesh->mTangents[i].y,
                                              graphicsMesh->mTangents[i].z);

                tangents.emplace_back(glm::normalize(tangent));
            }
            else
            {
                tangents.emplace_back(0.0f, 0.0f, 0.0f);
            }

            // Use only first set of texture coordinates.
            if(graphicsMesh->mTextureCoords[0])
            {
                textureCoords.emplace_back(graphicsMesh->mTextureCoords[0][i].x,
                                           graphicsMesh->mTextureCoords[0][i].y);
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
        for(int i = 0; i < graphicsMesh->mNumFaces; ++i) // Every face MUST be a triangle !!!!
        {
            indices.emplace_back(graphicsMesh->mFaces[i].mIndices[0]);
            indices.emplace_back(graphicsMesh->mFaces[i].mIndices[1]);
            indices.emplace_back(graphicsMesh->mFaces[i].mIndices[2]);
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

        // Material.
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
                texturePath = filePath.substr(0, filePath.find_last_of('/'));
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
                texturePath = filePath.substr(0, filePath.find_last_of('/'));
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

    std::vector<std::shared_ptr<SimpleObject>> SimpleObject::loadManyModelsFromOneFile(const char* filePath, SceneObjectGroups sceneGroup)
    {
        std::vector<std::shared_ptr<SimpleObject>> objects;
        std::shared_ptr<SimpleObject> obj;

        BR_INFO("Load many simple objects from one file: %s", filePath);

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

        for(int i = 0; i < scene->mNumMeshes; ++i)
        {
            obj = std::make_shared<SimpleObject>(filePath, scene, scene->mMeshes[i], sceneGroup);
            objects.push_back(obj);
        }

        return objects;
    }
}
