#include "SimpleObject.h"
#include "beryll/utils/File.h"

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
}
