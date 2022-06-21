#pragma once

#include "LibsHeaders.h"
#include "CppHeaders.h"
#include "Beryll/Core/Log.h"

namespace Utils
{
    class Common
    {
    public:
        Common() = delete;

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
    };
}