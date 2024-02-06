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

            if(m_material2) // If material 2 exist we need that to return UV into 0...1 range for blend texture.
            {
                m_internalShader->set1Float("addToUVCoords", m_addToUVCoords);
                m_internalShader->set1Float("UVCoordsMultiplier", m_UVCoordsMultiplier);
            }
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

        float UVSmallestX = std::numeric_limits<float>::max();
        float UVBiggestX = std::numeric_limits<float>::min();
        float UVSmallestY = std::numeric_limits<float>::max();
        float UVBiggestY = std::numeric_limits<float>::min();

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

                if(graphicsMesh->mTextureCoords[0][i].x < UVSmallestX)
                    UVSmallestX = graphicsMesh->mTextureCoords[0][i].x;
                if(graphicsMesh->mTextureCoords[0][i].x > UVBiggestX)
                    UVBiggestX = graphicsMesh->mTextureCoords[0][i].x;

                if(graphicsMesh->mTextureCoords[0][i].y < UVSmallestY)
                    UVSmallestY = graphicsMesh->mTextureCoords[0][i].y;
                if(graphicsMesh->mTextureCoords[0][i].y > UVBiggestY)
                    UVBiggestY = graphicsMesh->mTextureCoords[0][i].y;
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

        float UVXRange = glm::distance(UVSmallestX, UVBiggestX);
        float UVYRange = glm::distance(UVSmallestY, UVBiggestY);
        if(UVXRange < UVYRange)
        {
            m_addToUVCoords = std::abs(UVSmallestY);
            m_UVCoordsMultiplier = 1.0f / UVYRange;
        }
        else
        {
            m_addToUVCoords = std::abs(UVSmallestX);
            m_UVCoordsMultiplier = 1.0f / UVXRange;
        }

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

        // Load Material 1. At least diffuse texture of material 1 must exist.
        if(graphicsMesh->mMaterialIndex >= 0)
        {
            m_material1 = BeryllUtils::Common::loadMaterial1(scene->mMaterials[graphicsMesh->mMaterialIndex], filePath);

            m_internalShader->activateDiffuseTextureMat1();

            if(m_material1.specTexture)
                m_internalShader->activateSpecularTextureMat1();

            if(m_material1.normalMapTexture)
            {
                m_internalShader->activateNormalMapTextureMat1();

                BR_INFO("%s", "Create tangents buffer because model has normal map.");
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

    void SimpleObject::addMaterial2(const std::string& diffusePath,
                                             const std::string& specularPath,
                                             const std::string& normalMapPath,
                                             const std::string& blendTexturePath)
    {
        m_material2 = BeryllUtils::Common::loadMaterial2(diffusePath, specularPath, normalMapPath, blendTexturePath);

        BR_ASSERT(((m_material1.diffTexture != nullptr && m_material2->diffTexture != nullptr)),
                  "%s", "m_material1 and m_material2 both must have diffTexture.");

        BR_ASSERT(((m_material1.specTexture != nullptr && m_material2->specTexture != nullptr) ||
                   (m_material1.specTexture == nullptr && m_material2->specTexture == nullptr)),
                  "%s", "m_material1 and m_material2 both must have specTexture or both dont.");

        BR_ASSERT(((m_material1.normalMapTexture != nullptr && m_material2->normalMapTexture != nullptr) ||
                   (m_material1.normalMapTexture == nullptr && m_material2->normalMapTexture == nullptr)),
                  "%s", "m_material1 and m_material2 both must have normalMapTexture or both dont.");

        BR_ASSERT(((m_material2->blendTexture != nullptr)), "%s", "m_material2 must have blendTexture.");

        // Create different shader for two materials.
        m_internalShader = Renderer::createShader(BeryllConstants::simpleObjTwoMaterialsDefaultVertexPath.data(),
                                                  BeryllConstants::simpleObjTwoMaterialsDefaultFragmentPath.data());
        m_internalShader->bind();
        m_internalShader->activateDiffuseTextureMat1();

        if(m_material1.specTexture)
            m_internalShader->activateSpecularTextureMat1();

        if(m_material1.normalMapTexture)
            m_internalShader->activateNormalMapTextureMat1();

        m_internalShader->activateDiffuseTextureMat2();

        if(m_material2->specTexture)
            m_internalShader->activateSpecularTextureMat2();

        if(m_material2->normalMapTexture)
            m_internalShader->activateNormalMapTextureMat2();

        m_internalShader->activateBlendTextureMat2();

        BR_INFO("%s", "Loaded material 2 and created new shader.");
    }
}
