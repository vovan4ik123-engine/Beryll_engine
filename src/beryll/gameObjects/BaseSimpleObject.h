#pragma once

#include "SceneObject.h"

namespace Beryll
{
    class BaseSimpleObject : public SceneObject
    {
    public:
        ~BaseSimpleObject() override;

        void updateBeforePhysics() override;
        void updateAfterPhysics() override;
        void draw() override;

    protected:
        void loadGraphicsMesh(const std::string& filePath, const aiScene* scene, const aiMesh* graphicsMesh);
    };
}
