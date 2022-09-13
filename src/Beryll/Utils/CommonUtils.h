#pragma once

#include "LibsHeaders.h"
#include "CppHeaders.h"
#include "Beryll/Core/Constants.h"

namespace Utils
{
    class Common
    {
    public:
        Common() = delete;
        ~Common() = delete;

        static int generateID()
        {
            ++m_id;
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

        // return angle in range between 0 and PI
        static float getAngleInRadians(const glm::vec3& v1, const glm::vec3& v2)
        {
            return glm::acos(glm::dot(v1, v2));
        }

        // return angle in range between 0 and 180
        static float getAngleInDegrees(const glm::vec3& v1, const glm::vec3& v2)
        {
            return glm::degrees(glm::acos(glm::dot(v1, v2)));
        }

        // return axis around which start vec3 was rotated to be dest vec3
        // in right-handed coordinate system
        static glm::vec3 getRotationAxis(const glm::vec3& start, const glm::vec3& dest)
        {
            return glm::normalize(glm::cross(start, dest));
        }

        // return true if v2 is on right side from v1
        //  v1  v2      v1
        //  ^  ^        ^
        //  | /         |
        //  |/      OR  |-----> v2

        // return false if on left side
        //          v1
        //          ^
        //          |
        // v2 <-----|
        static bool getIsVectorOnRightSide(const glm::vec3& v1, const glm::vec3& v2)
        {
            const glm::vec3 rightVector = glm::normalize(glm::cross(v1, Constants::worldUp));

            if(glm::acos(glm::dot(rightVector, v2)) < glm::half_pi<float>())
                return true;
            else
                return false;
        }

        // return glm::quat that need to make dest from start
        static glm::quat getRotationBetweenVectors(const glm::vec3& start, const glm::vec3& dest)
        {
            float cosAngle = glm::dot(start, dest);

            if(cosAngle > 0.9999f) // angle is less than 0.81 degrees
            {
                // vectors are parallel

                return glm::quat(1.0f, 0.0f, 0.0f, 0.0f); // no rotation
            }

            if(cosAngle < -0.9999f) // angle is more than 179.19 degrees
            {
                // vectors are opposite

                float cosAngleStartAndY = glm::dot(start, glm::vec3(0.0f, 1.0f, 0.0f));
                if(cosAngleStartAndY > 0.9f || cosAngleStartAndY < -0.9f)
                {
                    // vectors are close to Y axis
                    // get cross product with X
                    glm::vec3 axis = glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), start);
                    return glm::angleAxis(glm::pi<float>(), glm::normalize(axis));
                }
                else
                {
                    // get cross product with Y
                    glm::vec3 axis = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), start);
                    return glm::angleAxis(glm::pi<float>(), glm::normalize(axis));
                }
            }

            float angleRadians = glm::acos(cosAngle);
            glm::vec3 axis = glm::cross(start, dest);

            return glm::angleAxis(angleRadians, glm::normalize(axis));
        }

    private:
        static int m_id;
    };
}