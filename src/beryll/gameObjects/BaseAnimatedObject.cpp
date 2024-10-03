#include "BaseAnimatedObject.h"
#include "beryll/core/TimeStep.h"
#include "beryll/utils/Quaternion.h"
#include "beryll/renderer/Camera.h"
#include "beryll/utils/File.h"
#include "beryll/renderer/Renderer.h"
#include "beryll/core/RandomGenerator.h"

namespace Beryll
{
    std::map<const std::string, std::pair<std::shared_ptr<Assimp::Importer>, const aiScene*>> BaseAnimatedObject::m_importersScenes;

    BaseAnimatedObject::BaseAnimatedObject(const char* filePath,
                                           SceneObjectGroups sceneGroup) : m_modelPath(filePath)
    {
        const auto search = m_importersScenes.find(m_modelPath);
        if(search != m_importersScenes.end())
        {
            // Scene from same file already was loaded. use it.
            BR_INFO("Use loaded before animated object: %s", filePath);
            m_scene = search->second.second;
        }
        else
        {
            BR_INFO("Load animated object: %s", filePath);
            std::shared_ptr<Assimp::Importer> importer = std::make_shared<Assimp::Importer>();
            const aiScene* scene = nullptr;

            uint32_t bufferSize = 0;
            char *buffer = BeryllUtils::File::readToBuffer(filePath, &bufferSize);

            scene = importer->ReadFileFromMemory(buffer, bufferSize,
                                                 aiProcess_Triangulate | aiProcess_FlipUVs |
                                                 aiProcess_JoinIdenticalVertices | aiProcess_CalcTangentSpace);
            delete[] buffer;
            if(!scene || !scene->mRootNode || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE)
            {
                BR_ASSERT(false, "Scene loading error for file: %s", filePath);
            }

            m_scene = scene;
            m_importersScenes.emplace(m_modelPath, std::make_pair(importer, scene));
        }

        m_sceneObjectGroup = sceneGroup;
        m_isAnimatedObject = true;

        m_globalInverseMatrix = m_scene->mRootNode->mTransformation;
        m_globalInverseMatrix.Inverse();

        for(int i = 0; i < m_scene->mNumMeshes; ++i)
        {
            std::string meshName = m_scene->mMeshes[i]->mName.C_Str();

            if(meshName.find("Collision") != std::string::npos)
            {
                // CollisionMesh will loaded in subclass AnimatedCollidingObject.
                continue;
            }

            BR_ASSERT((m_scene->HasAnimations() && m_scene->mMeshes[i]->mNumBones > 0),
                      "Colliding animated object must have animation + bone:%s" , m_scene->mMeshes[i]->mName.C_Str());

            // Prepare vectors.
            std::vector<glm::vec3> vertices;
            std::vector<glm::vec3> normals;
            std::vector<glm::vec3> tangents;
            std::vector<glm::vec2> textureCoords;
            std::vector<glm::ivec4> boneIDs;
            std::vector<glm::vec4> boneWeights;
            std::vector<uint32_t> indices;
            vertices.reserve(m_scene->mMeshes[i]->mNumVertices);
            normals.reserve(m_scene->mMeshes[i]->mNumVertices);
            tangents.reserve(m_scene->mMeshes[i]->mNumVertices);
            textureCoords.reserve(m_scene->mMeshes[i]->mNumVertices);
            boneIDs.resize(m_scene->mMeshes[i]->mNumVertices, glm::ivec4{-1, -1, -1, -1}); // NUM_BONES_PER_VERTEX
            boneWeights.resize(m_scene->mMeshes[i]->mNumVertices,glm::vec4{-1.0f, -1.0f, -1.0f, -1.0f}); // NUM_BONES_PER_VERTEX
            indices.reserve(m_scene->mMeshes[i]->mNumFaces * 3);

            float UVSmallestX = std::numeric_limits<float>::max();
            float UVBiggestX = std::numeric_limits<float>::min();
            float UVSmallestY = std::numeric_limits<float>::max();
            float UVBiggestY = std::numeric_limits<float>::min();

            // Vertices.
            for(int g = 0; g < m_scene->mMeshes[i]->mNumVertices; ++g)
            {
                vertices.emplace_back(m_scene->mMeshes[i]->mVertices[g].x,
                                      m_scene->mMeshes[i]->mVertices[g].y,
                                      m_scene->mMeshes[i]->mVertices[g].z);

                if(m_scene->mMeshes[i]->mNormals)
                {
                    glm::vec3 normal = glm::vec3(m_scene->mMeshes[i]->mNormals[g].x,
                                                 m_scene->mMeshes[i]->mNormals[g].y,
                                                 m_scene->mMeshes[i]->mNormals[g].z);

                    normals.emplace_back(glm::normalize(normal));
                }
                else
                {
                    normals.emplace_back(0.0f, 0.0f, 0.0f);
                }

                if(m_scene->mMeshes[i]->mTangents)
                {
                    glm::vec3 tangent = glm::vec3(m_scene->mMeshes[i]->mTangents[g].x,
                                                  m_scene->mMeshes[i]->mTangents[g].y,
                                                  m_scene->mMeshes[i]->mTangents[g].z);

                    tangents.emplace_back(glm::normalize(tangent));
                }
                else
                {
                    tangents.emplace_back(0.0f, 0.0f, 0.0f);
                }

                // Use only first set of texture coordinates.
                if(m_scene->mMeshes[i]->mTextureCoords[0])
                {
                    textureCoords.emplace_back(m_scene->mMeshes[i]->mTextureCoords[0][g].x,
                                               m_scene->mMeshes[i]->mTextureCoords[0][g].y);

                    if(m_scene->mMeshes[i]->mTextureCoords[0][g].x < UVSmallestX)
                        UVSmallestX = m_scene->mMeshes[i]->mTextureCoords[0][g].x;
                    if(m_scene->mMeshes[i]->mTextureCoords[0][g].x > UVBiggestX)
                        UVBiggestX = m_scene->mMeshes[i]->mTextureCoords[0][g].x;

                    if(m_scene->mMeshes[i]->mTextureCoords[0][g].y < UVSmallestY)
                        UVSmallestY = m_scene->mMeshes[i]->mTextureCoords[0][g].y;
                    if(m_scene->mMeshes[i]->mTextureCoords[0][g].y > UVBiggestY)
                        UVBiggestY = m_scene->mMeshes[i]->mTextureCoords[0][g].y;
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

            // Bones.
            m_boneCount = m_scene->mMeshes[i]->mNumBones;
            m_bonesMatrices.reserve(m_boneCount);
            m_boneNameIndex.reserve(m_boneCount);

            for(int g = 0; g < m_boneCount; ++g)
            {
                std::string boneName = m_scene->mMeshes[i]->mBones[g]->mName.C_Str();
                BR_ASSERT((boneName.length() > 3 &&
                           boneName[0] == 'B' &&
                           boneName[1] == 'o' &&
                           boneName[2] == 'n' &&
                           boneName[3] == 'e'), "%s", "Bone name must starts with Bone......");

                for(const std::pair<std::string, uint32_t>& element : m_boneNameIndex)
                {
                    BR_ASSERT((element.first != boneName), "Many bones have same name in one model: %s", filePath);
                }

                m_bonesMatrices.emplace_back(); // add empty element to back
                m_bonesMatrices.back().offsetMatrix = m_scene->mMeshes[i]->mBones[g]->mOffsetMatrix;
                m_boneNameIndex.emplace_back(boneName, g);

                // Collect all vertices to which bone has impact.
                for(int j = 0; j < m_scene->mMeshes[i]->mBones[g]->mNumWeights; ++j)
                {
                    uint32_t vertexIndex = m_scene->mMeshes[i]->mBones[g]->mWeights[j].mVertexId;
                    float weight = m_scene->mMeshes[i]->mBones[g]->mWeights[j].mWeight;

                    if(weight > 0.0f)
                    {
                        for(int k = 0; k < NUM_BONES_PER_VERTEX; ++k)
                        {
                            if(boneIDs[vertexIndex][k] == -1 && boneWeights[vertexIndex][k] == -1.0f)
                            {
                                boneIDs[vertexIndex][k] = g;
                                boneWeights[vertexIndex][k] = weight;
                                break;
                            }
                        }
                    }
                }
            }

            // Bubble sort boneIDs + boneWeights.
            for(int g = 0; g < boneWeights.size(); ++g)
            {
                float tmpWeight = 0.0f;
                int tmpBoneID = 0;

                for(int j = 0; j < NUM_BONES_PER_VERTEX; ++j)
                {
                    for(int k = 0; k + 1 < NUM_BONES_PER_VERTEX; ++k)
                    {
                        if(boneWeights[g][k] < boneWeights[g][k + 1])
                        {
                            // boneIDs and boneWeights have same size and structure.
                            tmpWeight = boneWeights[g][k];
                            tmpBoneID = boneIDs[g][k];

                            boneWeights[g][k] = boneWeights[g][k + 1];
                            boneIDs[g][k] = boneIDs[g][k + 1];

                            boneWeights[g][k + 1] = tmpWeight;
                            boneIDs[g][k + 1] = tmpBoneID;
                        }
                    }
                }
            }
            m_boneIDsBuffer = Renderer::createStaticVertexBuffer(boneIDs);
            m_boneWeightsBuffer = Renderer::createStaticVertexBuffer(boneWeights);

            // Indices.
            for(int g = 0; g < m_scene->mMeshes[i]->mNumFaces; ++g) // Every face MUST be a triangle !!!!
            {
                indices.emplace_back(m_scene->mMeshes[i]->mFaces[g].mIndices[0]);
                indices.emplace_back(m_scene->mMeshes[i]->mFaces[g].mIndices[1]);
                indices.emplace_back(m_scene->mMeshes[i]->mFaces[g].mIndices[2]);
            }
            BR_INFO("Indices count: %d", indices.size());
            m_indexBuffer = Renderer::createStaticIndexBuffer(indices);

            m_vertexArray = Renderer::createVertexArray();
            m_vertexArray->addVertexBuffer(m_vertexPosBuffer);
            m_vertexArray->addVertexBuffer(m_vertexNormalsBuffer);
            m_vertexArray->addVertexBuffer(m_textureCoordsBuffer);
            m_vertexArray->addVertexBuffer(m_boneIDsBuffer);
            m_vertexArray->addVertexBuffer(m_boneWeightsBuffer);
            // Tangents buffer will added if model has normal map.
            m_vertexArray->setIndexBuffer(m_indexBuffer);

            m_internalShader = Renderer::createShader(BeryllConstants::animatedObjDefaultVertexPath.data(),
                                                      BeryllConstants::animatedObjDefaultFragmentPath.data());
            m_internalShader->bind();

            // Load Material 1. At least diffuse texture of material 1 must exist.
            if(m_scene->mMeshes[i]->mMaterialIndex >= 0)
            {
                m_material1 = BeryllUtils::Common::loadMaterial1(m_scene->mMaterials[m_scene->mMeshes[i]->mMaterialIndex], filePath);

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

            // Animations.
            for(int g = 0; g < m_scene->mNumAnimations; ++g)
            {
                std::string animName = m_scene->mAnimations[g]->mName.C_Str();
                std::string::size_type startNameIndex = animName.find_last_of('|');
                if(startNameIndex != std::string::npos)
                {
                    animName = animName.substr(startNameIndex + 1);
                }

                m_animationNameIndex.emplace_back(animName, g);
                BR_INFO("Animation index: %d Name: %s Duration: %f", g, animName.c_str(), m_scene->mAnimations[g]->mDuration);
            }

            m_animStartTimeInSec = std::max(0.0f, TimeStep::getSecFromStart() - (RandomGenerator::getFloat() * 2.0f));

            m_ticksPerSecond = static_cast<float>(m_scene->mAnimations[0]->mTicksPerSecond);
            if(m_ticksPerSecond == 0.0f)
                m_ticksPerSecond = 24.0f;

            const aiNode *node = BeryllUtils::Common::findAinodeForAimesh(m_scene, m_scene->mRootNode, m_scene->mMeshes[i]->mName);
            if(node)
            {
                glm::mat4 modelMatrix = BeryllUtils::Matrix::aiToGlm(node->mTransformation);
                // Check scale. Should be 1.
                glm::vec3 scale = BeryllUtils::Matrix::getScaleFrom4x4Glm(modelMatrix);
                BR_ASSERT((scale.x > 0.9999f && scale.x < 1.0001f &&
                           scale.y > 0.9999f && scale.y < 1.0001f &&
                           scale.z > 0.9999f && scale.z < 1.0001f), "%s", "Scale should be baked to 1 in modeling tool.");

                m_totalRotation = BeryllUtils::Matrix::getRotationFrom4x4Glm(modelMatrix);
                m_origin = BeryllUtils::Matrix::getTranslationFrom4x4Glm(modelMatrix);
            }
        }
    }

    BaseAnimatedObject::~BaseAnimatedObject()
    {
        disableForEver();
    }

    void BaseAnimatedObject::updateBeforePhysics()
    {

    }

    void BaseAnimatedObject::updateAfterPhysics()
    {
        if(m_collisionFlag == CollisionFlags::DYNAMIC)
        {
            m_physicsTransforms = Physics::getTransforms(m_ID);

            m_totalRotation = glm::normalize(m_physicsTransforms.rotation);
            m_origin = m_physicsTransforms.origin;
        }

        // Should be here. Can be called in multi threading way. draw() will be called in single thread.
        if(getIsEnabledDraw())
        {
            calculateTransforms();
        }
    }

    void BaseAnimatedObject::draw()
    {
        if(useInternalShader)
        {
            m_internalShader->bind();
            m_internalShader->setMatrix4x4Float("MVPMatrix", Camera::getViewProjection() * getModelMatrix());

            for(int i = 0; i < m_boneCount; ++i)
            {
                m_boneMatrixNameInShader = "bonesMatrices[";
                m_boneMatrixNameInShader += std::to_string(i);
                m_boneMatrixNameInShader += "]";
                m_internalShader->setMatrix4x4Float(m_boneMatrixNameInShader.c_str(), m_bonesMatrices[i].finalWorldTransform);
            }

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
    
    void BaseAnimatedObject::calculateTransforms()
    {
        float timeInTicks = (TimeStep::getSecFromStart() - m_animStartTimeInSec) * m_ticksPerSecond;
        float animTime = std::fmodf(timeInTicks, static_cast<float>(m_scene->mAnimations[m_currentAnimIndex]->mDuration));

        if(m_playAnimOneTime)
        {
            if(animTime > m_animOneTimeLastFrameTime || m_animStartTimeInSec + m_animTimeInSec < TimeStep::getSecFromStart())
            {
                m_playAnimOneTime = false;
                // Prepare default animation.
                m_currentAnimIndex = m_defaultAnimIndex;
                m_animStartTimeInSec = TimeStep::getSecFromStart();
                timeInTicks = 0.0f;
                animTime = 0.0f;
            }
        }

        aiMatrix4x4 identity;
        readNodeHierarchy(animTime, m_scene->mRootNode, identity);
    }

    void BaseAnimatedObject::readNodeHierarchy(const float animationTime, const aiNode* node, const aiMatrix4x4& parentTransform)
    {
        aiMatrix4x4 nodeTransform; // identity

        const aiNodeAnim* nodeAnim = findNodeAnimByName(m_scene->mAnimations[m_currentAnimIndex], node->mName);

        if(nodeAnim)
        {
            BR_ASSERT((nodeAnim->mNumScalingKeys == nodeAnim->mNumPositionKeys), "%s", "mNumScalingKeys != mNumPositionKeys");
            BR_ASSERT((nodeAnim->mNumScalingKeys == nodeAnim->mNumRotationKeys), "%s", "mNumScalingKeys != mNumRotationKeys");

            uint32_t currentFrameIndex = 0;
            for(int i = nodeAnim->mNumPositionKeys - 1; i >= 0; --i)
            {
                if(animationTime > nodeAnim->mPositionKeys[i].mTime || i == 0)
                {
                    currentFrameIndex = i;
                    break;
                }
            }

            uint32_t nextFrameIndex = currentFrameIndex + 1;
            if(nextFrameIndex >= nodeAnim->mNumPositionKeys)
                nextFrameIndex = 0; // Last frame was played, jump to first again.

            float currentFrameStartTime = static_cast<float>(nodeAnim->mPositionKeys[currentFrameIndex].mTime);
            float currentFrameEndTime = static_cast<float>(nodeAnim->mPositionKeys[nextFrameIndex].mTime);
            if(currentFrameStartTime > currentFrameEndTime)
                currentFrameEndTime = static_cast<float>(m_scene->mAnimations[m_currentAnimIndex]->mDuration);

            BR_ASSERT((animationTime >= currentFrameStartTime && animationTime <= currentFrameEndTime),
                      "animationTime must be between currentFrameStartTime and currentFrameEndTime animationTime: %f, currentFrameStartTime: %f, currentFrameEndTime: %f",
                      animationTime, currentFrameStartTime, currentFrameEndTime);

            float deltaTime = currentFrameEndTime - currentFrameStartTime;
            // factor = how much time passed between current and next frame in range 0...1
            float factor = (animationTime - static_cast<float>(nodeAnim->mPositionKeys[currentFrameIndex].mTime)) / deltaTime;

            aiMatrix4x4 scalingMatr = interpolateScaling(nodeAnim, currentFrameIndex, nextFrameIndex, factor);
            aiMatrix4x4 rotationMatr = interpolateRotation(nodeAnim, currentFrameIndex, nextFrameIndex, factor);
            aiMatrix4x4 translationMatr = interpolatePosition(nodeAnim, currentFrameIndex, nextFrameIndex, factor);

            nodeTransform = translationMatr * rotationMatr * scalingMatr;
        }

        aiMatrix4x4 globalTransform = parentTransform * nodeTransform;

        // node_name = bone_name = animation->chanel->node_name(nodeAnim contains node_name of affected node)
        for(const std::pair<std::string, uint32_t>& element : m_boneNameIndex)
        {
            if(element.first == node->mName.C_Str())
            {
                // element.second = boneIndex
                m_bonesMatrices[element.second].finalWorldTransform = m_globalInverseMatrix * globalTransform * m_bonesMatrices[element.second].offsetMatrix;
                break;
            }
        }

        for(int i = 0; i < node->mNumChildren; ++i)
        {
            readNodeHierarchy(animationTime, node->mChildren[i], globalTransform);
        }
    }

    const aiNodeAnim* BaseAnimatedObject::findNodeAnimByName(const aiAnimation* animation, const aiString& nodeName)
    {
        // channel in animation it is aiNodeAnim (aiNodeAnim has transformation for node/bone with same name)
        // contains 3 arrays (scale/rotations/translations) for transform one node/bone in all frames
        // sequential frame number is index for these arrays
        // nodeAnim->mScalingKeys[0].mValue = scaling transform for frame 0 for node/bone named same as nodeAnim->mNodeName
        // numChannels == numBones

        if(nodeName.length < 4 || // use only aiNodeAnim which belong to bones
           nodeName.data[0] != 'B' || // Bones names must starts with Bone.......
           nodeName.data[1] != 'o' ||
           nodeName.data[2] != 'n' ||
           nodeName.data[3] != 'e')
        {
            return nullptr;
        }

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

    const aiNodeAnim* BaseAnimatedObject::findNodeAnimAny(const aiAnimation* animation)
    {
        for(int i = 0; i < animation->mNumChannels; ++i)
        {
            const aiNodeAnim* nodeAnim = animation->mChannels[i];

            if(nodeAnim->mNodeName.data[0] == 'B' &&
               nodeAnim->mNodeName.data[1] == 'o' &&
               nodeAnim->mNodeName.data[2] == 'n' &&
               nodeAnim->mNodeName.data[3] == 'e')
            {
                return nodeAnim;
            }
        }

        return nullptr;
    }

    aiMatrix4x4 BaseAnimatedObject::interpolatePosition(const aiNodeAnim* nodeAnim, const uint32_t currentFrameIndex, const uint32_t nextFrameIndex, const float factor)
    {
        aiMatrix4x4 posMatr;

        if(nodeAnim->mNumPositionKeys == 1)
        {
            aiMatrix4x4::Translation(nodeAnim->mPositionKeys[0].mValue, posMatr);
            return posMatr;
        }

        if(nodeAnim->mPositionKeys[currentFrameIndex].mValue == nodeAnim->mPositionKeys[nextFrameIndex].mValue)
        {
            // Dont need interpolate.
            aiMatrix4x4::Translation(nodeAnim->mPositionKeys[currentFrameIndex].mValue, posMatr);
            return posMatr;
        }

        const aiVector3D& start = nodeAnim->mPositionKeys[currentFrameIndex].mValue;
        const aiVector3D& end = nodeAnim->mPositionKeys[nextFrameIndex].mValue;
        const aiVector3D deltaVector = end - start;

        aiMatrix4x4::Translation(start + deltaVector * factor, posMatr);
        return posMatr;
    }

    aiMatrix4x4 BaseAnimatedObject::interpolateRotation(const aiNodeAnim* nodeAnim, const uint32_t currentFrameIndex, const uint32_t nextFrameIndex, const float factor)
    {
        if(nodeAnim->mNumRotationKeys == 1)
            return aiMatrix4x4(nodeAnim->mRotationKeys[0].mValue.GetMatrix());

        if(nodeAnim->mRotationKeys[currentFrameIndex].mValue == nodeAnim->mRotationKeys[nextFrameIndex].mValue)
        {
            // Dont need interpolate.
            return aiMatrix4x4(nodeAnim->mRotationKeys[currentFrameIndex].mValue.GetMatrix());
        }

        const aiQuaternion& start = nodeAnim->mRotationKeys[currentFrameIndex].mValue;
        const aiQuaternion& end = nodeAnim->mRotationKeys[nextFrameIndex].mValue;

        BR_ASSERT((((start.x * start.x + start.y * start.y + start.z * start.z + start.w * start.w) > 0.9999f) &&
                   ((start.x * start.x + start.y * start.y + start.z * start.z + start.w * start.w) < 1.0001f)), "%s", "start quaternion must be unit");
        BR_ASSERT((((end.x * end.x + end.y * end.y + end.z * end.z + end.w * end.w) > 0.9999f) &&
                   ((end.x * end.x + end.y * end.y + end.z * end.z + end.w * end.w) < 1.0001f)), "%s", "end quaternion must be unit");
        return aiMatrix4x4(BeryllUtils::Quaternion::nlerp(start, end, factor).GetMatrix());
    }

    aiMatrix4x4 BaseAnimatedObject::interpolateScaling(const aiNodeAnim* nodeAnim, const uint32_t currentFrameIndex, const uint32_t nextFrameIndex, const float factor)
    {
        aiMatrix4x4 scaleMatrix;

        if(nodeAnim->mNumScalingKeys == 1)
        {
            aiMatrix4x4::Scaling(nodeAnim->mScalingKeys[0].mValue, scaleMatrix);
            return scaleMatrix;
        }

        if(nodeAnim->mScalingKeys[currentFrameIndex].mValue == nodeAnim->mScalingKeys[nextFrameIndex].mValue)
        {
            // Dont need interpolate.
            aiMatrix4x4::Scaling(nodeAnim->mScalingKeys[currentFrameIndex].mValue, scaleMatrix);
            return scaleMatrix;
        }

        const aiVector3D& start = nodeAnim->mScalingKeys[currentFrameIndex].mValue;
        const aiVector3D& end = nodeAnim->mScalingKeys[nextFrameIndex].mValue;
        aiVector3D deltaVector = end - start;

        aiMatrix4x4::Scaling(start + deltaVector * factor, scaleMatrix);
        return scaleMatrix;
    }

    void BaseAnimatedObject::setCurrentAnimationByName(const char* name, bool playOneTime, bool startEvenIfSameAnimPlaying, bool randomizeAnimStartTime)
    {
        if(m_currentAnimName == name && !startEvenIfSameAnimPlaying) { return; }

        for(const std::pair<std::string, int>& anim : m_animationNameIndex)
        {
            if(anim.first == name)
            {
                m_currentAnimIndex = anim.second;
                m_currentAnimName = name;
                m_playAnimOneTime = playOneTime;
                if(m_playAnimOneTime)
                {
                    const aiNodeAnim* nodeAnim = findNodeAnimAny(m_scene->mAnimations[m_currentAnimIndex]);
                    BR_ASSERT((nodeAnim != nullptr), "%s", "Can not find any node anim.");
                    BR_ASSERT((nodeAnim->mNumPositionKeys  > 0), "%s", "Node anim does not have keys.");
                    m_animOneTimeLastFrameTime = nodeAnim->mPositionKeys[nodeAnim->mNumPositionKeys - 1].mTime;
                }
                m_animStartTimeInSec = TimeStep::getSecFromStart();
                if(randomizeAnimStartTime)
                    m_animStartTimeInSec = std::max(0.0f, TimeStep::getSecFromStart() - (RandomGenerator::getFloat() * 2.0f));

                m_animTimeInSec = static_cast<float>(m_scene->mAnimations[m_currentAnimIndex]->mDuration) / m_ticksPerSecond;
                return;
            }
        }

        BR_ASSERT(false, "Animation with name does not exists: %s", name);
    }

    void BaseAnimatedObject::setCurrentAnimationByIndex(int index, bool playOneTime, bool startEvenIfSameAnimPlaying, bool randomizeAnimStartTime)
    {
        BR_ASSERT((index >= 0 && index < m_animationNameIndex.size()), "Animation with index does not exists: %d", index);

        if(m_currentAnimIndex == index && !startEvenIfSameAnimPlaying) { return; }

        if(index >= 0 && index < m_animationNameIndex.size())
        {
            m_currentAnimIndex = index;
            m_playAnimOneTime = playOneTime;
            if(m_playAnimOneTime)
            {
                const aiNodeAnim* nodeAnim = findNodeAnimAny(m_scene->mAnimations[m_currentAnimIndex]);
                BR_ASSERT((nodeAnim != nullptr), "%s", "Can not find any node anim.");
                BR_ASSERT((nodeAnim->mNumPositionKeys  > 0), "%s", "Node anim does not have keys.");
                m_animOneTimeLastFrameTime = nodeAnim->mPositionKeys[nodeAnim->mNumPositionKeys - 1].mTime;
            }
            m_animStartTimeInSec = TimeStep::getSecFromStart();
            if(randomizeAnimStartTime)
                m_animStartTimeInSec = std::max(0.0f, TimeStep::getSecFromStart() - (RandomGenerator::getFloat() * 2.0f));

            m_animTimeInSec = static_cast<float>(m_scene->mAnimations[m_currentAnimIndex]->mDuration) / m_ticksPerSecond;
        }
    }

    void BaseAnimatedObject::setDefaultAnimationByName(const char* name)
    {
        for(const std::pair<std::string, int>& anim : m_animationNameIndex)
        {
            if(anim.first == name)
            {
                m_defaultAnimIndex = anim.second;
                return;
            }
        }

        BR_ASSERT(false, "Animation with name does not exists: %s", name);
    }

    void BaseAnimatedObject::setDefaultAnimationByIndex(int index)
    {
        BR_ASSERT((index >= 0 && index < m_animationNameIndex.size()), "Animation with index does not exists: %d", index);

        if(index >= 0 && index < m_animationNameIndex.size())
        {
            m_defaultAnimIndex = index;
        }
    }
}
