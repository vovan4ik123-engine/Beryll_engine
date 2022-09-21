#include "CollidingAnimatedObject.h"
#include "Beryll/Utils/File.h"
#include "Beryll/Utils/Matrix.h"
#include "Beryll/Utils/Quaternion.h"
#include "Beryll/Utils/CommonUtils.h"
#include "Beryll/Renderer/Camera.h"
#include "Beryll/Core/Window.h"
#include "Beryll/Core/TimeStep.h"
#include "Beryll/Core/Timer.h"

namespace Beryll
{
    CollidingAnimatedObject::CollidingAnimatedObject(const char* modelPath,
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
        BR_INFO("Loading colliding animated object:{0}", modelPath);

        uint32_t bufferSize = 0;
        char *buffer = Utils::File::readToBuffer(modelPath, &bufferSize);

        m_scene = m_importer.ReadFileFromMemory(buffer, bufferSize,
                                                aiProcess_Triangulate |
                                                aiProcess_SortByPType |
                                                aiProcess_FlipUVs);

        if (!m_scene || !m_scene->mRootNode || m_scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE)
        {
            BR_ASSERT(false, "Scene loading error for file:{0}", modelPath);
        }

        BR_INFO("Num meshes:{0}", m_scene->mNumMeshes);
        BR_ASSERT((m_scene->mNumMeshes == 2),
                  "Colliding animated object MUST contain 2 meshes {0}. For draw and physics simulation", modelPath);

        m_globalInverseMatrix = m_scene->mRootNode->mTransformation;
        m_globalInverseMatrix.Inverse();

        for(int i = 0; i < m_scene->mNumMeshes; ++i)
        {
            std::string meshName = m_scene->mMeshes[i]->mName.C_Str();

            if(meshName.find("Collision") != std::string::npos)
            {
                m_hasCollisionObject = true;
                m_collisionGroup = collGroup;

                processCollisionMesh(m_scene->mMeshes[i], meshName, collisionMass, wantCollisionCallBack, collFlag, collGroup, collMask);
                continue;
            }

            BR_ASSERT((m_scene->HasAnimations()) && (m_scene->mMeshes[i]->mNumBones > 0),
                      "Colliding animated object must have animation + bone");

            // prepare vectors
            std::vector<glm::vec3> vertices;
            std::vector<glm::vec3> normals;
            std::vector<glm::vec2> textureCoords;
            std::vector<glm::ivec4> boneIDs;
            std::vector<glm::vec4> boneWeights;
            std::vector<uint32_t> indices;
            vertices.reserve(m_scene->mMeshes[i]->mNumVertices);
            normals.reserve(m_scene->mMeshes[i]->mNumVertices);
            textureCoords.reserve(m_scene->mMeshes[i]->mNumVertices);
            boneIDs.resize(m_scene->mMeshes[i]->mNumVertices, glm::ivec4(-1, -1, -1, -1)); // NUM_BONES_PER_VERTEX
            boneWeights.resize(m_scene->mMeshes[i]->mNumVertices,glm::vec4(-1.0f, -1.0f, -1.0f, -1.0f)); // NUM_BONES_PER_VERTEX
            indices.reserve(m_scene->mMeshes[i]->mNumFaces * 3);

            // vertices
            for(int g = 0; g < m_scene->mMeshes[i]->mNumVertices; ++g)
            {
                vertices.emplace_back(m_scene->mMeshes[i]->mVertices[g].x,
                                      m_scene->mMeshes[i]->mVertices[g].y,
                                      m_scene->mMeshes[i]->mVertices[g].z);

                if (m_scene->mMeshes[i]->mNormals)
                {
                    normals.emplace_back(m_scene->mMeshes[i]->mNormals[g].x,
                                         m_scene->mMeshes[i]->mNormals[g].y,
                                         m_scene->mMeshes[i]->mNormals[g].z);
                } else
                    {
                    normals.emplace_back(0.0f, 0.0f, 0.0f);
                }

                // use only first set of texture coordinates
                if (m_scene->mMeshes[i]->mTextureCoords[0])
                {
                    textureCoords.emplace_back(m_scene->mMeshes[i]->mTextureCoords[0][g].x,
                                               m_scene->mMeshes[i]->mTextureCoords[0][g].y);
                } else
                    {
                    textureCoords.emplace_back(0.0f, 0.0f);
                }
            }
            m_vertexPosBuffer = Renderer::createVertexBuffer(vertices);
            m_vertexNormalsBuffer = Renderer::createVertexBuffer(normals);
            m_textureCoordsBuffer = Renderer::createVertexBuffer(textureCoords);

            // bones
            m_boneCount = m_scene->mMeshes[i]->mNumBones;
            m_bonesMatrices.reserve(m_boneCount);

            for(int g = 0; g < m_boneCount; ++g)
            {
                std::string boneName = m_scene->mMeshes[i]->mBones[g]->mName.C_Str();

                auto iter = m_boneNameIndex.find(boneName);
                if (iter != m_boneNameIndex.end())
                {
                    BR_ASSERT(false, "Many bones have same name in one model:{0}", modelPath);
                }
                else
                {
                    m_bonesMatrices.emplace_back(); // add empty element to back
                    m_bonesMatrices.back().offsetMatrix = m_scene->mMeshes[i]->mBones[g]->mOffsetMatrix;
                    m_boneNameIndex.insert(std::make_pair(boneName, g));
                }

                // collect all vertices to which bone has impact
                for (int j = 0; j < m_scene->mMeshes[i]->mBones[g]->mNumWeights; ++j)
                {
                    uint32_t vertexIndex = m_scene->mMeshes[i]->mBones[g]->mWeights[j].mVertexId;
                    float weight = m_scene->mMeshes[i]->mBones[g]->mWeights[j].mWeight;

                    for (int k = 0; k < NUM_BONES_PER_VERTEX; ++k)
                    {
                        if (boneIDs[vertexIndex][k] == -1 && boneWeights[vertexIndex][k] == -1.0f)
                        {
                            boneIDs[vertexIndex][k] = g;
                            boneWeights[vertexIndex][k] = weight;
                            break;
                        }
                    }
                }
            }
            m_boneIDsBuffer = Renderer::createVertexBuffer(boneIDs);
            m_boneWeightsBuffer = Renderer::createVertexBuffer(boneWeights);

            // indices
            for (int g = 0; g < m_scene->mMeshes[i]->mNumFaces; ++g) // every face MUST be a triangle !!!!
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
            m_vertexArray->addVertexBuffer(m_boneIDsBuffer);
            m_vertexArray->addVertexBuffer(m_boneWeightsBuffer);
            m_vertexArray->setIndexBuffer(m_indexBuffer);

            m_shader = Renderer::createShader(vertexPath, fragmentPath);

            // material
            if (m_scene->mMeshes[i]->mMaterialIndex >= 0)
            {
                aiMaterial *material = m_scene->mMaterials[m_scene->mMeshes[i]->mMaterialIndex];

                const std::string mP = modelPath;
                BR_ASSERT((mP.find_last_of('/') != std::string::npos), "Texture + model must be in folder:{0}", mP);

                std::string texturePath;

                if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0 && diffSampler != nullptr)
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

                if (material->GetTextureCount(aiTextureType_SPECULAR) > 0 && specSampler != nullptr)
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

            // animations
            for(int g = 0; g < m_scene->mNumAnimations; ++g)
            {
                m_animationNameIndex.insert(std::make_pair(m_scene->mAnimations[g]->mName.C_Str(), g));
                BR_INFO("Have animation {0} with name:{1}", g, m_scene->mAnimations[g]->mName.C_Str());
            }

            const aiNode *node = Utils::Common::findAinodeForAimesh(m_scene, m_scene->mRootNode, m_scene->mMeshes[i]->mName);
            if (node)
            {
                m_modelMatrix = Utils::Matrix::aiToGlm(node->mTransformation);
            }

            m_scaleMatrix = glm::scale(glm::mat4{1.0f}, Utils::Matrix::getScaleFrom4x4Glm(m_modelMatrix));
            m_origin = Utils::Matrix::getTranslationFrom4x4Glm(m_modelMatrix);
        }
    }

    CollidingAnimatedObject::~CollidingAnimatedObject()
    {

    }

    void CollidingAnimatedObject::updateBeforePhysics()
    {

    }

    void CollidingAnimatedObject::updateAfterPhysics()
    {
        m_physicsTransforms = Physics::getTransforms(m_ID);

        m_origin = m_physicsTransforms.origin;

        m_translateMatrix = glm::translate(glm::mat4{1.0f}, m_physicsTransforms.origin);
        m_rotateMatrix = glm::toMat4(m_physicsTransforms.rotation);

        m_modelMatrix = m_translateMatrix * m_rotateMatrix * m_scaleMatrix;
    }

    void CollidingAnimatedObject::draw()
    {
        calculateTransforms();

        m_MVP = Camera::get3DCamera() * m_modelMatrix;

        m_shader->bind();
        m_shader->setMatrix4x4Float("MVP_matrix", m_MVP);

        for(int i = 0; i < m_boneCount; ++i)
        {
            m_boneMatrixNameInShader = "bonesMatrices[";
            m_boneMatrixNameInShader += std::to_string(i);
            m_boneMatrixNameInShader += "]";
            m_shader->setMatrix4x4Float(m_boneMatrixNameInShader.c_str(), m_bonesMatrices[i].finalWorldTransform);
        }

        if(m_diffTexture) { m_diffTexture->bind(); }
        if(m_specTexture) { m_specTexture->bind(); }

        m_vertexArray->bind();
        m_vertexArray->draw();
        m_vertexArray->unBind();

        if(m_diffTexture) { m_diffTexture->unBind(); }
        if(m_specTexture) { m_specTexture->unBind(); }

        m_shader->unBind();
    }

    void CollidingAnimatedObject::playSound()
    {

    }

    void CollidingAnimatedObject::calculateTransforms()
    {
        float tickPerSecond = static_cast<float>(m_scene->mAnimations[m_currentAnimIndex]->mTicksPerSecond);
        float timeInTicks = TimeStep::getSecFromStart() * ((tickPerSecond == 0.0f) ? 24 : tickPerSecond);
        float animTime = fmod(timeInTicks, static_cast<float>(m_scene->mAnimations[m_currentAnimIndex]->mDuration));

        aiMatrix4x4 identity;
        readNodeHierarchy(animTime, m_scene->mRootNode, identity);
    }

    void CollidingAnimatedObject::readNodeHierarchy(const float animationTime, const aiNode* node, const aiMatrix4x4& parentTransform)
    {
        const aiNodeAnim* nodeAnim = findNodeAnim(m_scene->mAnimations[m_currentAnimIndex], node->mName);

        aiMatrix4x4 nodeTransform = node->mTransformation;

        if(nodeAnim)
        {
            // BR_INFO("nodeAnim->mNumScalingKeys:{0}", nodeAnim->mNumScalingKeys);
            BR_ASSERT((nodeAnim->mNumScalingKeys == nodeAnim->mNumPositionKeys), "mNumScalingKeys != mNumPositionKeys");
            BR_ASSERT((nodeAnim->mNumScalingKeys == nodeAnim->mNumRotationKeys), "mNumScalingKeys != mNumRotationKeys");

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
            BR_ASSERT((nextFrameIndex < nodeAnim->mNumPositionKeys), "nextFrameIndex ! < nodeAnim->mNumPositionKeys");

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

    const aiNodeAnim* CollidingAnimatedObject::findNodeAnim(const aiAnimation* animation, const aiString& nodeName)
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

    aiMatrix4x4 CollidingAnimatedObject::interpolatePosition(const aiNodeAnim* nodeAnim, const int currentFrameIndex, const int nextFrameIndex, const float factor)
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

    aiMatrix4x4 CollidingAnimatedObject::interpolateRotation(const aiNodeAnim* nodeAnim, const int currentFrameIndex, const int nextFrameIndex, const float factor)
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

    aiMatrix4x4 CollidingAnimatedObject::interpolateScaling(const aiNodeAnim* nodeAnim, const int currentFrameIndex, const int nextFrameIndex, const float factor)
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

    void CollidingAnimatedObject::setAnimation(const char* name)
    {
        const auto iter = m_animationNameIndex.find(name);
        if(iter != m_animationNameIndex.end())
        {
            m_currentAnimIndex = iter->second;
        }
        else
        {
            BR_ASSERT(false, "Animation with name:{0} does not exist", name);
        }
    }

    void CollidingAnimatedObject::processCollisionMesh(const aiMesh* mesh,
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