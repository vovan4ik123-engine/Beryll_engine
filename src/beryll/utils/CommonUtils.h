#pragma once

#include "LibsHeaders.h"
#include "CppHeaders.h"

#include "beryll/core/BeryllConstants.h"
#include "beryll/core/Log.h"
#include "beryll/renderer/Material.h"

namespace BeryllUtils
{
    class Common
    {
    public:
        Common() = delete;
        ~Common() = delete;

        // Max IDs count = int::max = 2,147,483,647.
        static int generateID()
        {
            ++m_id;
            BR_ASSERT((m_id < std::numeric_limits<decltype(m_id)>::max()), "%s", "Reached limit of objects IDs");
            return m_id;
        }

        static int getLastGeneratedID()
        {
            return m_id;
        }

        static const aiNode* findAinodeForAimesh(const aiScene* scene, const aiNode* rootNode, const aiString& name)
        {
            for(int i = 0; i < rootNode->mNumMeshes; ++i)
            {
                if(scene->mMeshes[rootNode->mMeshes[i]]->mName == name)
                {
                    return rootNode;
                }
            }

            for(int i = 0; i < rootNode->mNumChildren; ++i)
            {
                const aiNode* n = findAinodeForAimesh(scene, rootNode->mChildren[i], name);
                if(n)
                {
                    return n;
                }
            }

            return nullptr;
        }

        // Return angle in range between 0 and PI.
        static float getAngleInRadians(const glm::vec3& v1, const glm::vec3& v2)
        {
            return glm::acos(glm::dot(v1, v2));
        }

        // Return angle in range between 0 and 180.
        static float getAngleInDegrees(const glm::vec3& v1, const glm::vec3& v2)
        {
            return glm::degrees(glm::acos(glm::dot(v1, v2)));
        }

        // Return axis around which start vec3 was rotated to be dest vec3
        // in right-handed coordinate system.
        static glm::vec3 getRotationAxis(const glm::vec3& start, const glm::vec3& dest)
        {
            return glm::normalize(glm::cross(start, dest));
        }

        enum class VectorSide
        {
            ERROR,
            ON_RIGHT_SIDE,
            ON_LEFT_SIDE
        };
        // Return ON_RIGHT_SIDE if v2 is on right side from v1.
        //  v1  v2      v1
        //  ^  ^        ^
        //  | /         |
        //  |/      OR  |-----> v2

        // Return ON_LEFT_SIDE if on left side.
        //          v1
        //          ^
        //          |
        // v2 <-----|

        // Return ERROR if can not find right vector.
        static VectorSide getIsVectorOnRightSide(const glm::vec3& v1, const glm::vec3& v2)
        {
            float dotProduct = glm::dot(v1, BeryllConstants::worldUp);
            if(dotProduct == 1.0f || dotProduct == -1.0f)
                return VectorSide::ERROR; // v1 and BeryllConstants::worldUp are parallel, we can not find right vector.

            glm::vec3 rightVector = glm::normalize(glm::cross(v1, BeryllConstants::worldUp));

            if(glm::acos(glm::dot(rightVector, v2)) < glm::half_pi<float>())
                return VectorSide::ON_RIGHT_SIDE;
            else
                return VectorSide::ON_LEFT_SIDE;
        }

        static glm::vec3 getRightVector(const glm::vec3& v1)
        {
            BR_ASSERT((glm::dot(v1, BeryllConstants::worldUp) != 1.0f &&
                       glm::dot(v1, BeryllConstants::worldUp) != -1.0f), "%s", "Vectors are parallel. Use getIsVectorOnRightSide() to check.");

            return glm::normalize(glm::cross(v1, BeryllConstants::worldUp));
        }

        static glm::vec3 getLeftVector(const glm::vec3& v1)
        {
            BR_ASSERT((glm::dot(v1, BeryllConstants::worldUp) != 1.0f &&
                       glm::dot(v1, BeryllConstants::worldUp) != -1.0f), "%s", "Vectors are parallel. Use getIsVectorOnRightSide() to check.");

            return glm::normalize(glm::cross(BeryllConstants::worldUp, v1));
        }

        // Return angle in range between 0 and PI.
        static float getAngleInRadians(const glm::vec2& v1, const glm::vec2& v2)
        {
            return glm::acos(glm::dot(v1, v2));
        }

        // Return angle in range between 0 and 180.
        static float getAngleInDegrees(const glm::vec2& v1, const glm::vec2& v2)
        {
            return glm::degrees(glm::acos(glm::dot(v1, v2)));
        }

        // return glm::quat that need to make dest from start
        // static glm::quat getRotationBetweenVectors(const glm::vec3& start, const glm::vec3& dest)
        // use glm::rotation

        static Beryll::Material1 loadMaterial1(aiMaterial* material, const std::string& filePath);

    private:
        static int m_id;
    };
}
