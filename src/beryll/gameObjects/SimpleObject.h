#pragma once

#include "BaseSimpleObject.h"

namespace Beryll
{
    // Not animated object, not participates in physics simulation.
    class SimpleObject : public BaseSimpleObject
    {
    public:
        SimpleObject() = delete;
        /*
         * filePath - path to model file (.DAE or .FBX). start path from first folder inside assets/
         * sceneGroup - game specific group to which this scene object belong
         */
        SimpleObject(const char* filePath,
                     SceneObjectGroups sceneGroup);
        SimpleObject(const std::string& filePath,
                     const aiScene* scene,
                     const aiMesh* graphicsMesh,
                     SceneObjectGroups sceneGroup);
        ~SimpleObject() override;

        // All loaded objects will have same sceneGroup.
        static std::vector<std::shared_ptr<SimpleObject>> loadManyModelsFromOneFile(const char* filePath, SceneObjectGroups sceneGroup);
    };
}
