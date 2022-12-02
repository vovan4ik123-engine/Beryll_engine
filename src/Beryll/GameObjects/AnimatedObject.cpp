#include "AnimatedObject.h"
#include "Beryll/Utils/File.h"
#include "Beryll/Utils/Matrix.h"
#include "Beryll/Utils/Quaternion.h"
#include "Beryll/Utils/CommonUtils.h"
#include "Beryll/Renderer/Camera.h"
#include "Beryll/Renderer/Renderer.h"
#include "Beryll/Core/Window.h"
#include "Beryll/Core/TimeStep.h"
#include "Beryll/Core/Timer.h"

namespace Beryll
{
    std::map<const std::string, std::pair<std::shared_ptr<Assimp::Importer>, const aiScene*>> AnimatedObject::m_importersScenes;

    AnimatedObject::AnimatedObject(const char* modelPath) : m_modelPath(modelPath)
    {
        const auto search = m_importersScenes.find(m_modelPath);
        if(search != m_importersScenes.end())
        {
            // scene from same file already was loaded. use it
            m_scene = search->second.second;
        }
        else
        {
            std::shared_ptr<Assimp::Importer> importer = std::make_shared<Assimp::Importer>();
            const aiScene* scene = nullptr;

            BR_INFO("Loading animated object:%s", modelPath);
            uint32_t bufferSize = 0;
            char *buffer = Utils::File::readToBuffer(modelPath, &bufferSize);

            scene = importer->ReadFileFromMemory(buffer, bufferSize,
                                                 aiProcess_Triangulate |
                                                 aiProcess_FlipUVs |
                                                 aiProcess_CalcTangentSpace);
            delete[] buffer;
            if(!scene || !scene->mRootNode || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE)
            {
                BR_ASSERT(false, "Scene loading error for file:%s", modelPath);
            }

            BR_ASSERT((scene->mNumMeshes == 1),
                      "Animated object:%s MUST contain only 1 mesh. Combine into one if you have many", modelPath);

            BR_ASSERT((scene->HasAnimations()) && (scene->mMeshes[0]->mNumBones > 0),
                      "%s", "Animated object must have animation + bone");

            m_scene = scene;

            m_importersScenes.emplace(m_modelPath, std::make_pair(importer, scene));
        }

        m_globalInverseMatrix = m_scene->mRootNode->mTransformation;
        m_globalInverseMatrix.Inverse();

        // prepare vectors
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec3> tangents;
        std::vector<glm::vec2> textureCoords;
        std::vector<glm::ivec4> boneIDs;
        std::vector<glm::vec4> boneWeights;
        std::vector<uint32_t> indices;
        vertices.reserve(m_scene->mMeshes[0]->mNumVertices);
        normals.reserve(m_scene->mMeshes[0]->mNumVertices);
        tangents.reserve(m_scene->mMeshes[0]->mNumVertices);
        textureCoords.reserve(m_scene->mMeshes[0]->mNumVertices);
        boneIDs.resize(m_scene->mMeshes[0]->mNumVertices, glm::ivec4(-1, -1, -1, -1)); // NUM_BONES_PER_VERTEX
        boneWeights.resize(m_scene->mMeshes[0]->mNumVertices, glm::vec4(-1.0f, -1.0f, -1.0f, -1.0f)); // NUM_BONES_PER_VERTEX
        indices.reserve(m_scene->mMeshes[0]->mNumFaces * 3);

        // vertices
        for(int i = 0; i < m_scene->mMeshes[0]->mNumVertices; ++i)
        {
            vertices.emplace_back(m_scene->mMeshes[0]->mVertices[i].x,
                                  m_scene->mMeshes[0]->mVertices[i].y,
                                  m_scene->mMeshes[0]->mVertices[i].z);

            if(m_scene->mMeshes[0]->mNormals)
            {
                glm::vec3 normal = glm::vec3(m_scene->mMeshes[0]->mNormals[i].x,
                                             m_scene->mMeshes[0]->mNormals[i].y,
                                             m_scene->mMeshes[0]->mNormals[i].z);

                normals.emplace_back(glm::normalize(normal));
            }
            else
            {
                normals.emplace_back(0.0f, 0.0f, 0.0f);
            }

            if(m_scene->mMeshes[0]->mTangents)
            {
                glm::vec3 tangent = glm::vec3(m_scene->mMeshes[0]->mTangents[i].x,
                                              m_scene->mMeshes[0]->mTangents[i].y,
                                              m_scene->mMeshes[0]->mTangents[i].z);

                tangents.emplace_back(glm::normalize(tangent));
            }
            else
            {
                tangents.emplace_back(0.0f, 0.0f, 0.0f);
            }

            // use only first set of texture coordinates
            if(m_scene->mMeshes[0]->mTextureCoords[0])
            {
                textureCoords.emplace_back(m_scene->mMeshes[0]->mTextureCoords[0][i].x,
                                           m_scene->mMeshes[0]->mTextureCoords[0][i].y);
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

        // bones
        m_boneCount = m_scene->mMeshes[0]->mNumBones;
        m_bonesMatrices.reserve(m_boneCount);

        for(int i = 0; i < m_boneCount; ++i)
        {
            std::string boneName = m_scene->mMeshes[0]->mBones[i]->mName.C_Str();

            auto iter = m_boneNameIndex.find(boneName);
            if(iter != m_boneNameIndex.end())
            {
                BR_ASSERT(false, "Many bones have same name in one model:%s", modelPath);
            }
            else
            {
                m_bonesMatrices.emplace_back(); // add empty element to back
                m_bonesMatrices.back().offsetMatrix = m_scene->mMeshes[0]->mBones[i]->mOffsetMatrix;
                m_boneNameIndex.insert(std::make_pair(boneName, i));
            }

            // collect all vertices to which bone has impact
            for(int j = 0; j < m_scene->mMeshes[0]->mBones[i]->mNumWeights; ++j)
            {
                uint32_t vertexIndex = m_scene->mMeshes[0]->mBones[i]->mWeights[j].mVertexId;
                float weight = m_scene->mMeshes[0]->mBones[i]->mWeights[j].mWeight;

                for(int k = 0; k < NUM_BONES_PER_VERTEX; ++k)
                {
                    if(boneIDs[vertexIndex][k] == -1 && boneWeights[vertexIndex][k] == -1.0f)
                    {
                        boneIDs[vertexIndex][k] = i;
                        boneWeights[vertexIndex][k] = weight;
                        break;
                    }
                }
            }
        }
        m_boneIDsBuffer = Renderer::createStaticVertexBuffer(boneIDs);
        m_boneWeightsBuffer = Renderer::createStaticVertexBuffer(boneWeights);

        // indices
        for(int i = 0; i < m_scene->mMeshes[0]->mNumFaces; ++i) // every face MUST be a triangle !!!!
        {
            indices.emplace_back(m_scene->mMeshes[0]->mFaces[i].mIndices[0]);
            indices.emplace_back(m_scene->mMeshes[0]->mFaces[i].mIndices[1]);
            indices.emplace_back(m_scene->mMeshes[0]->mFaces[i].mIndices[2]);
        }
        m_indexBuffer = Renderer::createStaticIndexBuffer(indices);

        m_vertexArray = Renderer::createVertexArray();
        m_vertexArray->addVertexBuffer(m_vertexPosBuffer);
        m_vertexArray->addVertexBuffer(m_vertexNormalsBuffer);
        m_vertexArray->addVertexBuffer(m_textureCoordsBuffer);
        m_vertexArray->addVertexBuffer(m_boneIDsBuffer);
        m_vertexArray->addVertexBuffer(m_boneWeightsBuffer);
        m_vertexArray->addVertexBuffer(m_vertexTangentsBuffer);
        m_vertexArray->setIndexBuffer(m_indexBuffer);

        m_internalShader = Renderer::createShader(BeryllConstants::animatedObjDefaultVertexPath.data(),
                                                  BeryllConstants::animatedObjDefaultFragmentPath.data());
        m_internalShader->bind();

        // material
        if(m_scene->mMeshes[0]->mMaterialIndex >= 0)
        {
            aiMaterial *material = m_scene->mMaterials[m_scene->mMeshes[0]->mMaterialIndex];

            BR_ASSERT((m_modelPath.find_last_of('/') != std::string::npos), "Texture + model must be in folder:%s", m_modelPath.c_str());

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
                texturePath = m_modelPath.substr(0, m_modelPath.find_last_of('/'));
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
                texturePath = m_modelPath.substr(0, m_modelPath.find_last_of('/'));
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
                texturePath = m_modelPath.substr(0, m_modelPath.find_last_of('/'));
                texturePath += '/';
                texturePath += textName2;
                BR_INFO("Normal map texture here:%s", texturePath.c_str());

                m_normalMapTexture = Renderer::createTexture(texturePath.c_str(), TextureType::NORMAL_MAP_TEXTURE);
                m_internalShader->activateNormalMapTexture();
            }
        }

        // animations
        for(int i = 0; i < m_scene->mNumAnimations; ++i)
        {
            m_animationNameIndex.insert(std::make_pair(m_scene->mAnimations[i]->mName.C_Str(), i));
            BR_INFO("Have animation %d with name:%s", i, m_scene->mAnimations[i]->mName.C_Str());
        }

        const aiNode* node = Utils::Common::findAinodeForAimesh(m_scene, m_scene->mRootNode, m_scene->mMeshes[0]->mName);
        if(node)
        {
            m_modelMatrix = Utils::Matrix::aiToGlm(node->mTransformation);
        }

        m_origin = Utils::Matrix::getTranslationFrom4x4Glm(m_modelMatrix);
    }

    AnimatedObject::~AnimatedObject()
    {

    }

    void AnimatedObject::updateBeforePhysics()
    {

    }

    void AnimatedObject::updateAfterPhysics()
    {

        calculateTransforms();
    }

    void AnimatedObject::draw()
    {
        if(useInternalShader)
        {
            m_internalShader->bind();
            m_MVP = Camera::getViewProjection() * m_modelMatrix;
            m_internalShader->setMatrix4x4Float("MVPMatrix", m_MVP);

            for(int i = 0; i < m_boneCount; ++i)
            {
                m_boneMatrixNameInShader = "bonesMatrices[";
                m_boneMatrixNameInShader += std::to_string(i);
                m_boneMatrixNameInShader += "]";
                m_internalShader->setMatrix4x4Float(m_boneMatrixNameInShader.c_str(), m_bonesMatrices[i].finalWorldTransform);
            }
        }

        if(m_diffTexture && useInternalTextures) { m_diffTexture->bind(); }
        if(m_specTexture && useInternalTextures) { m_specTexture->bind(); }
        if(m_normalMapTexture && useInternalTextures) { m_normalMapTexture->bind(); }

        m_vertexArray->bind();
        m_vertexArray->draw();
    }

    void AnimatedObject::playSound()
    {

    }

    void AnimatedObject::calculateTransforms()
    {
        float tickPerSecond = static_cast<float>(m_scene->mAnimations[m_currentAnimIndex]->mTicksPerSecond);
        float timeInTicks = TimeStep::getSecFromStart() * ((tickPerSecond == 0.0f) ? 24 : tickPerSecond);
        float animTime = fmod(timeInTicks, static_cast<float>(m_scene->mAnimations[m_currentAnimIndex]->mDuration));

        aiMatrix4x4 identity;
        readNodeHierarchy(animTime, m_scene->mRootNode, identity);
    }

    void AnimatedObject::readNodeHierarchy(const float animationTime, const aiNode* node, const aiMatrix4x4& parentTransform)
    {
        const aiNodeAnim* nodeAnim = findNodeAnim(m_scene->mAnimations[m_currentAnimIndex], node->mName);

        aiMatrix4x4 nodeTransform = node->mTransformation;

        if(nodeAnim)
        {
            BR_ASSERT((nodeAnim->mNumScalingKeys == nodeAnim->mNumPositionKeys), "%s", "mNumScalingKeys != mNumPositionKeys");
            BR_ASSERT((nodeAnim->mNumScalingKeys == nodeAnim->mNumRotationKeys), "%s", "mNumScalingKeys != mNumRotationKeys");

            int currentFrameIndex = 0;
            for(int i = 0; i < nodeAnim->mNumPositionKeys - 1; ++i) // will use i + 1
            {
                if(animationTime < nodeAnim->mPositionKeys[i + 1].mTime) // less than next = actual frame did not finish
                {
                    currentFrameIndex = i;
                    break;
                }
            }

            int nextFrameIndex = currentFrameIndex + 1;
            BR_ASSERT((nextFrameIndex < nodeAnim->mNumPositionKeys), "%s", "nextFrameIndex ! < nodeAnim->mNumPositionKeys");

            float deltaTime = static_cast<float>(nodeAnim->mPositionKeys[nextFrameIndex].mTime) -
                              static_cast<float>(nodeAnim->mPositionKeys[currentFrameIndex].mTime);
            // factor = how much time passed between current and next frame in range 0...1
            float factor = (animationTime - static_cast<float>(nodeAnim->mPositionKeys[currentFrameIndex].mTime)) / deltaTime;
            BR_ASSERT((factor >= 0.0f && factor <= 1.0f),
                      "Translation factor must be in range 0...1. Factor:{0}, mTime:{1}, currentFrameIndex:{2}",
                      factor, nodeAnim->mPositionKeys[currentFrameIndex].mTime, currentFrameIndex);

            aiMatrix4x4 scalingMatr = interpolateScaling(nodeAnim, currentFrameIndex, nextFrameIndex, factor);
            aiMatrix4x4 rotationMatr = interpolateRotation(nodeAnim, currentFrameIndex, nextFrameIndex, factor);
            aiMatrix4x4 translationMatr = interpolatePosition(nodeAnim, currentFrameIndex, nextFrameIndex, factor);

            nodeTransform = translationMatr * rotationMatr * scalingMatr;
        }

        aiMatrix4x4 globalTransform = parentTransform * nodeTransform;

        // node_name = bone_name = animation->chanel->node_name(nodeAnim contains node_name of affected node)
        auto iter = m_boneNameIndex.find(node->mName.C_Str());
        if(iter != m_boneNameIndex.end())
        {
            // iter->second = boneIndex
            m_bonesMatrices[iter->second].finalWorldTransform = m_globalInverseMatrix * globalTransform * m_bonesMatrices[iter->second].offsetMatrix;
        }

        for(int i = 0; i < node->mNumChildren; ++i)
        {
            readNodeHierarchy(animationTime, node->mChildren[i], globalTransform);
        }
    }

    const aiNodeAnim* AnimatedObject::findNodeAnim(const aiAnimation* animation, const aiString& nodeName)
    {
        // channel in animation it is aiNodeAnim (aiNodeAnim has transformation for node/bone with same name)
        // contains 3 arrays (scale/rotations/translations) for transform one node/bone in all frames
        // sequential frame number is index for these arrays
        // nodeAnim->mScalingKeys[0].mValue = scaling transform for frame 0 for node/bone named same as nodeAnim->mNodeName
        // numChannels == numBones
        for(int i = 0; i < animation->mNumChannels; ++i)
        {
            const aiNodeAnim* nodeAnim = animation->mChannels[i];
            if(nodeAnim->mNodeName == nodeName)
            {
                return nodeAnim;
            }
        }
        return nullptr;
    }

    aiMatrix4x4 AnimatedObject::interpolatePosition(const aiNodeAnim* nodeAnim, const int currentFrameIndex, const int nextFrameIndex, const float factor)
    {
        aiMatrix4x4 posMatr;

        if(nodeAnim->mNumPositionKeys == 1)
        {
            aiMatrix4x4::Translation(nodeAnim->mPositionKeys[0].mValue, posMatr);
            return posMatr;
        }

        if(nodeAnim->mPositionKeys[currentFrameIndex].mValue == nodeAnim->mPositionKeys[nextFrameIndex].mValue)
        {
            // dont need interpolate
            aiMatrix4x4::Translation(nodeAnim->mPositionKeys[currentFrameIndex].mValue, posMatr);
            return posMatr;
        }

        const aiVector3D& start = nodeAnim->mPositionKeys[currentFrameIndex].mValue;
        const aiVector3D& end = nodeAnim->mPositionKeys[nextFrameIndex].mValue;
        const aiVector3D deltaVector = end - start;

        aiMatrix4x4::Translation(start + deltaVector * factor, posMatr);
        return posMatr;
    }

    aiMatrix4x4 AnimatedObject::interpolateRotation(const aiNodeAnim* nodeAnim, const int currentFrameIndex, const int nextFrameIndex, const float factor)
    {
        if(nodeAnim->mNumRotationKeys == 1)
            return aiMatrix4x4(nodeAnim->mRotationKeys[0].mValue.GetMatrix());

        if(nodeAnim->mRotationKeys[currentFrameIndex].mValue == nodeAnim->mRotationKeys[nextFrameIndex].mValue)
        {
            // dont need interpolate
            return aiMatrix4x4(nodeAnim->mRotationKeys[currentFrameIndex].mValue.GetMatrix());
        }

        aiQuaternion& start = nodeAnim->mRotationKeys[currentFrameIndex].mValue;
        aiQuaternion& end = nodeAnim->mRotationKeys[nextFrameIndex].mValue;

        // Utils::Quaternion::nlerp() will normalize quaternions it takes by reference if they not
        return aiMatrix4x4(Utils::Quaternion::nlerp(start, end, factor).GetMatrix());
    }

    aiMatrix4x4 AnimatedObject::interpolateScaling(const aiNodeAnim* nodeAnim, const int currentFrameIndex, const int nextFrameIndex, const float factor)
    {
        aiMatrix4x4 scaleMatrix;

        if(nodeAnim->mNumScalingKeys == 1)
        {
            aiMatrix4x4::Scaling(nodeAnim->mScalingKeys[0].mValue, scaleMatrix);
            return scaleMatrix;
        }

        if(nodeAnim->mScalingKeys[currentFrameIndex].mValue == nodeAnim->mScalingKeys[nextFrameIndex].mValue)
        {
            // dont need interpolate
            aiMatrix4x4::Scaling(nodeAnim->mScalingKeys[currentFrameIndex].mValue, scaleMatrix);
            return scaleMatrix;
        }

        const aiVector3D& start = nodeAnim->mScalingKeys[currentFrameIndex].mValue;
        const aiVector3D& end = nodeAnim->mScalingKeys[nextFrameIndex].mValue;
        aiVector3D deltaVector = end - start;

        aiMatrix4x4::Scaling(start + deltaVector * factor, scaleMatrix);
        return scaleMatrix;
    }

    void AnimatedObject::setAnimation(const char* name)
    {
        const auto iter = m_animationNameIndex.find(name);
        if(iter != m_animationNameIndex.end())
        {
            m_currentAnimIndex = iter->second;
        }
        else
        {
            BR_ASSERT(false, "Animation with name:%s does not exist", name);
        }
    }
}
