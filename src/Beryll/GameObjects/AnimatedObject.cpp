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

    AnimatedObject::AnimatedObject(const char* modelPath,
                                   SceneObjectGroups objGroup) : m_modelPath(modelPath)
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

            BR_INFO("Loading animated object: %s", modelPath);
            uint32_t bufferSize = 0;
            char *buffer = Utils::File::readToBuffer(modelPath, &bufferSize);

            scene = importer->ReadFileFromMemory(buffer, bufferSize,
                                                 aiProcess_Triangulate |
                                                 aiProcess_FlipUVs |
                                                 aiProcess_CalcTangentSpace);
            delete[] buffer;
            if(!scene || !scene->mRootNode || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE)
            {
                BR_ASSERT(false, "Scene loading error for file: %s", modelPath);
            }

            BR_ASSERT((scene->mNumMeshes == 1),
                      "Animated object: %s MUST contain only 1 mesh. Combine into one if you have many", modelPath);

            BR_ASSERT((scene->HasAnimations() && scene->mMeshes[0]->mNumBones > 0),
                      "%s", "Animated object must have animation + bone");

            m_scene = scene;

            m_importersScenes.emplace(m_modelPath, std::make_pair(importer, scene));
        }

        m_sceneObjectGroup = objGroup;

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
        boneIDs.resize(m_scene->mMeshes[0]->mNumVertices, glm::ivec4{-1, -1, -1, -1}); // NUM_BONES_PER_VERTEX
        boneWeights.resize(m_scene->mMeshes[0]->mNumVertices, glm::vec4{-1.0f, -1.0f, -1.0f, -1.0f}); // NUM_BONES_PER_VERTEX
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
        m_boneNameIndex.reserve(m_boneCount);

        for(int i = 0; i < m_boneCount; ++i)
        {
            std::string boneName = m_scene->mMeshes[0]->mBones[i]->mName.C_Str();
            BR_ASSERT((boneName.length() > 3 &&
                       boneName[0] == 'B' &&
                       boneName[1] == 'o' &&
                       boneName[2] == 'n' &&
                       boneName[3] == 'e'), "Bone name must starts with Bone...... : %s", boneName.c_str());

            for(const std::pair<std::string, uint32_t>& element : m_boneNameIndex)
            {
                BR_ASSERT((element.first != boneName), "Many bones have same name in one model: %s", modelPath);
            }

            m_bonesMatrices.emplace_back(); // add empty element to back
            m_bonesMatrices.back().offsetMatrix = m_scene->mMeshes[0]->mBones[i]->mOffsetMatrix;
            m_boneNameIndex.emplace_back(boneName, i);

            // collect all vertices to which bone has impact
            for(int j = 0; j < m_scene->mMeshes[0]->mBones[i]->mNumWeights; ++j)
            {
                uint32_t vertexIndex = m_scene->mMeshes[0]->mBones[i]->mWeights[j].mVertexId;
                float weight = m_scene->mMeshes[0]->mBones[i]->mWeights[j].mWeight;

                if(weight > 0.0f)
                {
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

            BR_ASSERT((m_modelPath.find_last_of('/') != std::string::npos), "Texture + model must be in folder: %s", m_modelPath.c_str());

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
                texturePath = m_modelPath.substr(0, m_modelPath.find_last_of('/'));
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
                texturePath = m_modelPath.substr(0, m_modelPath.find_last_of('/'));
                texturePath += '/';
                texturePath += textName2;
                BR_INFO("Normal map texture here: %s", texturePath.c_str());

                m_normalMapTexture = Renderer::createTexture(texturePath.c_str(), TextureType::NORMAL_MAP_TEXTURE);
                m_internalShader->activateNormalMapTexture();
            }
        }

        // animations
        for(int i = 0; i < m_scene->mNumAnimations; ++i)
        {
            std::string animName = m_scene->mAnimations[i]->mName.C_Str();
            std::string::size_type startNameIndex = animName.find_last_of('|');
            if(startNameIndex != std::string::npos)
            {
                animName = animName.substr(startNameIndex + 1);
            }

            m_animationNameIndex.emplace_back(animName, i);
            BR_INFO("Animation index: %d Name: %s Duration: %f", i, animName.c_str(), m_scene->mAnimations[i]->mDuration);
        }

        const aiNode* node = Utils::Common::findAinodeForAimesh(m_scene, m_scene->mRootNode, m_scene->mMeshes[0]->mName);
        if(node)
        {
            m_modelMatrix = Utils::Matrix::aiToGlm(node->mTransformation);

            m_scaleMatrix = glm::scale(glm::mat4{1.0f}, Utils::Matrix::getScaleFrom4x4Glm(m_modelMatrix));
            m_rotateMatrix = glm::toMat4(Utils::Matrix::getRotationFrom4x4Glm(m_modelMatrix));
            m_origin = Utils::Matrix::getTranslationFrom4x4Glm(m_modelMatrix);
            m_translateMatrix = glm::translate(glm::mat4{1.0f}, m_origin);
        }
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
        aiMatrix4x4 nodeTransform; // identity

        const aiNodeAnim* nodeAnim = findNodeAnim(m_scene->mAnimations[m_currentAnimIndex], node->mName);

        if(nodeAnim)
        {
            BR_ASSERT((nodeAnim->mNumScalingKeys == nodeAnim->mNumPositionKeys), "%s", "mNumScalingKeys != mNumPositionKeys");
            BR_ASSERT((nodeAnim->mNumScalingKeys == nodeAnim->mNumRotationKeys), "%s", "mNumScalingKeys != mNumRotationKeys");

            uint32_t currentFrameIndex = 0;
            for(uint32_t i = nodeAnim->mNumPositionKeys - 1; i >= 0; --i)
            {
                if(animationTime > nodeAnim->mPositionKeys[i].mTime || i == 0)
                {
                    currentFrameIndex = i;
                    break;
                }
            }

            uint32_t nextFrameIndex = currentFrameIndex + 1;
            if(nextFrameIndex >= nodeAnim->mNumPositionKeys)
            {
                nextFrameIndex = 0;
            }

            float currentFrameStartTime = static_cast<float>(nodeAnim->mPositionKeys[currentFrameIndex].mTime);
            float currentFrameEndTime = static_cast<float>(nodeAnim->mPositionKeys[nextFrameIndex].mTime);
            if(currentFrameStartTime > currentFrameEndTime)
            {
                currentFrameEndTime = static_cast<float>(m_scene->mAnimations[m_currentAnimIndex]->mDuration);
            }

            BR_ASSERT((animationTime >= currentFrameStartTime && animationTime <= currentFrameEndTime),
                      "animationTime must be between currentFrameStartTime and currentFrameEndTime animationTime: %f, currentFrameStartTime: %f, currentFrameEndTime: %f",
                      animationTime, currentFrameStartTime, currentFrameEndTime);

            float deltaTime = currentFrameEndTime - currentFrameStartTime;
            // factor = how much time passed between current and next frame in range 0...1
            float factor = (animationTime - currentFrameStartTime) / deltaTime;

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

    const aiNodeAnim* AnimatedObject::findNodeAnim(const aiAnimation* animation, const aiString& nodeName)
    {
        // channel in animation it is aiNodeAnim (aiNodeAnim has transformation for node/bone with same name)
        // contains 3 arrays (scale/rotations/translations) for transform one node/bone in all frames
        // sequential frame number is index for these arrays
        // nodeAnim->mScalingKeys[0].mValue = scaling transform for frame 0 for node/bone named same as nodeAnim->mNodeName
        // numChannels == numBones

        if(nodeName.length < 4 || // use only aiNodeAnim which belong to bones
           nodeName.data[0] != 'B' || // Bones names must start with Bone.......
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

    aiMatrix4x4 AnimatedObject::interpolatePosition(const aiNodeAnim* nodeAnim, const uint32_t currentFrameIndex, const uint32_t nextFrameIndex, const float factor)
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

    aiMatrix4x4 AnimatedObject::interpolateRotation(const aiNodeAnim* nodeAnim, const uint32_t currentFrameIndex, const uint32_t nextFrameIndex, const float factor)
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

    aiMatrix4x4 AnimatedObject::interpolateScaling(const aiNodeAnim* nodeAnim, const uint32_t currentFrameIndex, const uint32_t nextFrameIndex, const float factor)
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

    void AnimatedObject::setAnimationByName(const char* name)
    {
        if(m_currentAnimName == name) { return; }

        for(const std::pair<std::string, uint32_t>& anim : m_animationNameIndex)
        {
            if(anim.first == name)
            {
                m_currentAnimIndex = anim.second;
                m_currentAnimName = name;
                return;
            }
        }
    }

    void AnimatedObject::setAnimationByIndex(uint32_t index)
    {
        if(m_currentAnimIndex == index) { return; }

        if(index < m_animationNameIndex.size())
        {
            m_currentAnimIndex = index;
        }
    }
}
